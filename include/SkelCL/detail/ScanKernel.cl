/*****************************************************************************
 * Copyright (c) 2011-2012 The SkelCL Team as listed in CREDITS.txt          *
 * http://skelcl.uni-muenster.de                                             *
 *                                                                           *
 * This file is part of SkelCL.                                              *
 * SkelCL is available under multiple licenses.                              *
 * The different licenses are subject to terms and condition as provided     *
 * in the files specifying the license. See "LICENSE.txt" for details        *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * SkelCL is free software: you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation, either version 3 of the License, or         *
 * (at your option) any later version. See "LICENSE-gpl.txt" for details.    *
 *                                                                           *
 * SkelCL is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
 * GNU General Public License for more details.                              *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * For non-commercial academic use see the license specified in the file     *
 * "LICENSE-academic.txt".                                                   *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * If you are interested in other licensing models, including a commercial-  *
 * license, please contact the author at michel.steuwer@uni-muenster.de      *
 *                                                                           *
 *****************************************************************************/

///
/// \file ScanKernel.cl
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///
 
R"(

typedef float SCL_TYPE_0;

#ifndef SCL_IDENTITY
#define SCL_IDENTITY (0.0)
#endif

//------------------------------------------------------------
// kernel__ExclusivePrefixScan
//
// Purpose : do a scan on a chunck of data.
//------------------------------------------------------------

// Define this to more rigorously avoid bank conflicts, even at the lower (root)
// levels of the tree.
// To avoid bank conflicts during the tree traversal, we need to add padding to
// the shared memory array every NUM_BANKS (16) elements.
// Note that due to the higher addressing overhead, performance is lower with
// ZERO_BANK_CONFLICTS enabled.
// It is provided as an example.
//#define ZERO_BANK_CONFLICTS 

// 16 banks on G80
#define NUM_BANKS 16
#define LOG_NUM_BANKS 4

#ifdef ZERO_BANK_CONFLICTS
#define CONFLICT_FREE_OFFSET(index) \
  ((index) >> LOG_NUM_BANKS + (index) >> (2*LOG_NUM_BANKS))
#else
#define CONFLICT_FREE_OFFSET(index) ((index) >> LOG_NUM_BANKS)
#endif

__kernel
void SCL_SCAN(__global const SCL_TYPE_0* currentInput,
              __global       SCL_TYPE_0* output,
              __local        SCL_TYPE_0* localBuffer,
              __global       SCL_TYPE_0* blockSums,
                       const uint        blockSumsSize)
{
  const uint gid = get_global_id(0);
  const uint tid = get_local_id(0);
  const uint bid = get_group_id(0);
  const uint lwz = get_local_size(0);

  // The local buffer has 2x the size of the local-work-size, because we manage
  // 2 scans at a time.
  const uint localBufferSize = lwz << 1;
  int offset = 1;

  const int tid2_0 = tid << 1;
  const int tid2_1 = tid2_0 + 1;

  const int gid2_0 = gid << 1;
  const int gid2_1 = gid2_0 + 1;

  // Cache the data in local memory

#ifdef SUPPORT_AVOID_BANK_CONFLICT
  uint ai  = tid;
  uint bi  = tid + lwz;
  uint gai = gid;
  uint gbi = gid + lwz;
  uint bankOffsetA = CONFLICT_FREE_OFFSET(ai); 
  uint bankOffsetB = CONFLICT_FREE_OFFSET(bi);
  localBuffer[ai + bankOffsetA]
    = (gai < blockSumsSize) ? currentInput[gai] : SCL_IDENTITY; 
  localBuffer[bi + bankOffsetB]
    = (gbi < blockSumsSize) ? currentInput[gbi] : SCL_IDENTITY;
#else
  localBuffer[tid2_0]
    = (gid2_0 < blockSumsSize) ? currentInput[gid2_0] : SCL_IDENTITY;
  localBuffer[tid2_1]
    = (gid2_1 < blockSumsSize) ? currentInput[gid2_1] : SCL_IDENTITY;
#endif

  // bottom-up (a.k.a. up-sweep phase)
  for(uint d = lwz; d > 0; d >>= 1) {
    barrier(CLK_LOCAL_MEM_FENCE);

    if (tid < d) {
#ifdef SUPPORT_AVOID_BANK_CONFLICT
      uint i = 2 * offset * tid;
      uint ai = i + offset - 1;
      uint bi = ai + offset;
      ai += CONFLICT_FREE_OFFSET(ai); // ai += ai / NUM_BANKS;
      bi += CONFLICT_FREE_OFFSET(bi); // bi += bi / NUM_BANKS;
#else
      // offset*(tid2_0+1)-1 = offset*(tid2_1+0)-1
      const uint ai = mad24(offset, (tid2_1+0), -1);
      // offset*(tid2_1+1)-1;
      const uint bi = mad24(offset, (tid2_1+1), -1);
#endif

      localBuffer[bi] = SCL_FUNC( localBuffer[bi], localBuffer[ai] );
    }
    offset <<= 1;
  }

  barrier(CLK_LOCAL_MEM_FENCE);

  if (tid < 1) {
#ifdef SUPPORT_AVOID_BANK_CONFLICT
    uint index = localBufferSize-1;
    index += CONFLICT_FREE_OFFSET(index);
    blockSums[bid] = localBuffer[index];
    localBuffer[index] = SCL_IDENTITY;
#else
    // We store the biggest value (the last) to the sum-block for later use.
    blockSums[bid] = localBuffer[localBufferSize-1];
    // Clear the last element
    localBuffer[localBufferSize - 1] = SCL_IDENTITY;
#endif
  }

  // top-down (a.k.a. down-sweep phase)
  for(uint d = 1; d < localBufferSize; d <<= 1) {
    offset >>= 1;
    barrier(CLK_LOCAL_MEM_FENCE);

    if (tid < d) {
#ifdef SUPPORT_AVOID_BANK_CONFLICT
      uint i = 2 * offset * tid;
      uint ai = i + offset - 1;
      uint bi = ai + offset;
      ai += CONFLICT_FREE_OFFSET(ai); // Apply an offset to the __local memory
      bi += CONFLICT_FREE_OFFSET(bi);
#else
      // offset*(tid2_0+1)-1 = offset*(tid2_1+0)-1
      const uint ai = mad24(offset, (tid2_1+0), -1);
      // offset*(tid2_1+1)-1;
      const uint bi = mad24(offset, (tid2_1+1), -1);
#endif

      SCL_TYPE_0 tmp = localBuffer[ai];
      localBuffer[ai] = localBuffer[bi];
      localBuffer[bi] = SCL_FUNC(localBuffer[bi], tmp);
    }
  }

  barrier(CLK_LOCAL_MEM_FENCE);

  // Copy back from the local buffer to the output array

#ifdef SUPPORT_AVOID_BANK_CONFLICT
  output[gai] = (gai < blockSumsSize) * localBuffer[ai + bankOffsetA];
  output[gbi] = (gbi < blockSumsSize) * localBuffer[bi + bankOffsetB];
#else
  if (gid2_0 < blockSumsSize) {
    output[gid2_0] = localBuffer[tid2_0];
  }
  if (gid2_1 < blockSumsSize) {
    output[gid2_1] = localBuffer[tid2_1];
  }
#endif
}

//------------------------------------------------------------
// kernel__ExclusivePrefixScan
//
// Purpose :
// Final step of large-array scan: combine basic inclusive scan with exclusive
// scan of top elements of input arrays.
//------------------------------------------------------------

__kernel void SCL_UNIFORM_COMBINATION(__global       SCL_TYPE_0* output,
                                      __global const SCL_TYPE_0* blockSums,
                                               const uint        outputSize)
{
        uint gid     = get_global_id(0) * 2;
  const uint tid     = get_local_id(0);
  const uint blockId = get_group_id(0);

  // Intel SDK fix
  //output[gid] = SCL_FUNC( output[gid], blockSums[blockId] );
  //output[gid+1] = SCL_FUNC( output[gid+1], blockSums[blockId] );

  __local SCL_TYPE_0 localBuffer[1];

#ifdef SUPPORT_AVOID_BANK_CONFLICT
  uint blockOffset = 1024 - 1;
  if (tid < 1) {
    localBuffer[0] = blockSums[blockId + blockOffset];
  }
#else
  if (tid < 1) {
    localBuffer[0] = blockSums[blockId];
  }
#endif

  barrier(CLK_LOCAL_MEM_FENCE);

#ifdef SUPPORT_AVOID_BANK_CONFLICT
  unsigned int address = blockId * get_local_size(0) * 2 + get_local_id(0); 

  output[address] = SCL_FUNC(output[address], localBuffer[0]);
  output[address + get_local_size(0)]
      = SCL_FUNC( output[address + get_local_size(0)],
                   (get_local_id(0) + get_local_size(0) < outputSize)
                   * localBuffer[0] );
#else
  if (gid < outputSize) {
    output[gid] = SCL_FUNC(output[gid], localBuffer[0]);
  }
  gid++;
  if (gid < outputSize) {
    output[gid] = SCL_FUNC(output[gid], localBuffer[0]);
  }
#endif
}

)"

