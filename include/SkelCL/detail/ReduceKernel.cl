
R"(

typedef float SCL_TYPE_0;

#ifndef SCL_IDENTITY
#define SCL_IDENTITY (0.0)
#endif


// ------------------------------------ Kernel 1 -----------------------------------------------


__kernel void SCL_REDUCE_1 (
    const __global SCL_TYPE_0* SCL_IN,
          __global SCL_TYPE_0* SCL_OUT,  
    const unsigned int         DATA_SIZE,
    const unsigned int         GLOBAL_SIZE)
{
    const unsigned int modul = GLOBAL_SIZE;

    SCL_TYPE_0 res = SCL_IDENTITY;
    int i = get_global_id(0);
        
    while ( i < DATA_SIZE )
    {
      res = res + SCL_IN[i];
      i = i + modul;
    }
      
    SCL_OUT[get_global_id(0)] = res;
}


// ------------------------------------ Kernel 2 -----------------------------------------------

__kernel void SCL_REDUCE_2 (
          __global SCL_TYPE_0* SCL_IN,
          __global SCL_TYPE_0* SCL_OUT,
    const unsigned int         DATA_SIZE,
    const unsigned int         GLOBAL_SIZE)    
{
    // ASSERT: DATA_SIZE <= get_local_size() ( == get_global_size() )

    int pos = get_local_id(0);
    int size = DATA_SIZE;
    
    int modul;
    int active;
    while ( size > 1)
    {  
      barrier(CLK_LOCAL_MEM_FENCE); // GLOBAL MEM FENCE
      modul  = ceil( size / 2.0f );
      active = floor( size / 2.0f );
      
      if ( pos < active )
        SCL_IN[pos] = SCL_IN[pos] + SCL_IN[ pos + modul ];
      
      size = ceil( size / 2.0f);
    }
        
    SCL_OUT[0] = SCL_IN[0];
}
                                                  
)"
