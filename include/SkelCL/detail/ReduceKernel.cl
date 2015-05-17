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


R"(

typedef float SCL_TYPE_0;

#ifndef SCL_IDENTITY
#define SCL_IDENTITY (0.0)
#endif


// ------------------------------------ Kernel 1 -----------------------------------------------

__kernel void SCL_REDUCE_1 (
    const __global SCL_TYPE_0* SCL_IN,
          __global SCL_TYPE_0* SCL_OUT,
    const unsigned int  DATA_SIZE,
    const unsigned int  GLOBAL_SIZE) 
{
    const int my_pos = get_global_id(0);
    if (my_pos > DATA_SIZE) return;
        
    const unsigned int modul = GLOBAL_SIZE;

    SCL_TYPE_0 res = SCL_IN[my_pos];
    int        i   = my_pos + modul;
        
    while ( i < DATA_SIZE )
    {
      res = SCL_FUNC( res, SCL_IN[i] );
      i = i + modul;
    }
    
    SCL_OUT[my_pos] = res;
}


// ------------------------------------ Kernel 2 -----------------------------------------------


__kernel void SCL_REDUCE_2 (
    const __global SCL_TYPE_0* SCL_IN,
          __global SCL_TYPE_0* SCL_OUT,
          __local  SCL_TYPE_0* LOCAL_BUF, // has size LOCAL_SIZE
          unsigned int         DATA_SIZE,
    const unsigned int         LOCAL_SIZE)    
{
    const int my_pos  = get_global_id(0);
    
    int modul;
    
    
    // preprocessing phase

    modul = LOCAL_SIZE;

    SCL_TYPE_0 res = SCL_IN[my_pos];
    int        i   = my_pos + modul;
        
    while ( i < DATA_SIZE )
    {
      res = SCL_FUNC( res, SCL_IN[i] );
      i   = i + modul;
    }
      
    LOCAL_BUF[my_pos] = res;


    DATA_SIZE = min( LOCAL_SIZE, DATA_SIZE );
    
    
    // less than LOCAL_SIZE elements remaining

    int active;
    
    while ( DATA_SIZE > 1)
    {  
      barrier(CLK_LOCAL_MEM_FENCE);
      
      modul  = ceil ( DATA_SIZE / 2.0f );
      active = floor( DATA_SIZE / 2.0f );
      
      if ( my_pos < active )
        LOCAL_BUF[my_pos] = SCL_FUNC( LOCAL_BUF[my_pos], LOCAL_BUF[ my_pos + modul ] );
      
      DATA_SIZE = ceil( DATA_SIZE / 2.0f);
    }

    SCL_OUT[0] = LOCAL_BUF[0];

}
                                                  
)"
