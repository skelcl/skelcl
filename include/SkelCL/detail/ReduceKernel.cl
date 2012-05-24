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
/// \file ReduceKernel.cl
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///
 
R"(

typedef float SCL_TYPE_0;

#ifndef SCL_IDENTITY
#define SCL_IDENTITY (0.0)
#endif

/* group size is power of 2 and not greater than 512 */
#ifndef GROUP_SIZE
#define GROUP_SIZE (64)
#endif

#define LOAD_GLOBAL(s, i) \
    ((__global const SCL_TYPE_0*)(s))[(size_t)(i)]

#define STORE_GLOBAL(s, i, v) \
    ((__global SCL_TYPE_0*)(s))[(size_t)(i)] = (v)

#define LOAD_LOCAL(s, i) \
    ((__local SCL_TYPE_0*)(s))[(size_t)(i)]

#define STORE_LOCAL(s, i, v) \
    ((__local SCL_TYPE_0*)(s))[(size_t)(i)] = (v)

#define ACCUM_LOCAL(s, i, j) \
{ \
    int x = ((__local SCL_TYPE_0*)(s))[(size_t)(i)]; \
    int y = ((__local SCL_TYPE_0*)(s))[(size_t)(j)]; \
    ((__local SCL_TYPE_0*)(s))[(size_t)(i)] = SCL_FUNC(x, y); \
}

__kernel void SCL_REDUCE(
    const __global SCL_TYPE_0* SCL_IN,
          __global SCL_TYPE_0* SCL_OUT,
          __local  SCL_TYPE_0* shared,
    const         unsigned int n /* number of elements to reduce */)
{
    const SCL_TYPE_0 identity = SCL_IDENTITY;

    const unsigned int local_id     = get_local_id(0);
    const unsigned int group_id     = get_group_id(0);
    const unsigned int group_size   = GROUP_SIZE;
    /* in one step every work_item processes 2 elements */
    const unsigned int step_size    = 2 * group_size;

    STORE_LOCAL(shared, local_id, identity);

    unsigned int i = group_id * step_size + local_id;
    int step = 0;
    while (i < n)
    {
        SCL_TYPE_0 a = LOAD_GLOBAL(SCL_IN, i);
        /* alternative introduce padding with group_size? */
        SCL_TYPE_0 b = (i+group_size < n) ? LOAD_GLOBAL(SCL_IN, i + group_size) : identity;
        SCL_TYPE_0 s = LOAD_LOCAL(shared, local_id);
        STORE_LOCAL(shared, local_id, SCL_FUNC(SCL_FUNC(a, b), s));
        i += (step_size) * get_num_groups(0); /* jump over all work_groups */
    }

    barrier(CLK_LOCAL_MEM_FENCE);
    #if (GROUP_SIZE >= 512)
    if (local_id < 256) { ACCUM_LOCAL(shared, local_id, local_id + 256); }
    #endif

    barrier(CLK_LOCAL_MEM_FENCE);
    #if (GROUP_SIZE >= 256)
    if (local_id < 128) { ACCUM_LOCAL(shared, local_id, local_id + 128); }
    #endif

    barrier(CLK_LOCAL_MEM_FENCE);
    #if (GROUP_SIZE >= 128)
    if (local_id <  64) { ACCUM_LOCAL(shared, local_id, local_id +  64); }
    #endif

    barrier(CLK_LOCAL_MEM_FENCE);
    #if (GROUP_SIZE >= 64)
    if (local_id <  32) { ACCUM_LOCAL(shared, local_id, local_id +  32); }
    #endif

    barrier(CLK_LOCAL_MEM_FENCE);
    #if (GROUP_SIZE >= 32)
    if (local_id <  16) { ACCUM_LOCAL(shared, local_id, local_id +  16); }
    #endif

    barrier(CLK_LOCAL_MEM_FENCE);
    #if (GROUP_SIZE >= 16)
    if (local_id <   8) { ACCUM_LOCAL(shared, local_id, local_id +   8); }
    #endif

    barrier(CLK_LOCAL_MEM_FENCE);
    #if (GROUP_SIZE >= 8)
    if (local_id <   4) { ACCUM_LOCAL(shared, local_id, local_id +   4); }
    #endif

    barrier(CLK_LOCAL_MEM_FENCE);
    #if (GROUP_SIZE >= 4)
    if (local_id <   2) { ACCUM_LOCAL(shared, local_id, local_id +   2); }
    #endif

    barrier(CLK_LOCAL_MEM_FENCE);
    #if (GROUP_SIZE >= 2)
    if (local_id <   1) { ACCUM_LOCAL(shared, local_id, local_id +   1); }
    #endif

    /* first work_item of the work_group stores the local result in the global memory */
    barrier(CLK_LOCAL_MEM_FENCE);
    if (get_local_id(0) == 0) {
        SCL_TYPE_0 v = LOAD_LOCAL(shared, 0);
        STORE_GLOBAL(SCL_OUT, group_id, v);
    }
}
)"
