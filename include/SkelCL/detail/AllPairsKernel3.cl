
R"(

typedef float SCL_TYPE_0;
typedef float SCL_TYPE_1;
typedef float SCL_TYPE_2;

#define SIZE 16

__kernel void SCL_ALLPAIRS(const __global SCL_TYPE_0* M,
                           const __global SCL_TYPE_1* N,
                                 __global SCL_TYPE_2* P, 
                           const unsigned int dimension,
                           const unsigned int height,
                           const unsigned int width) {
    __local SCL_TYPE_0 Ml[SIZE][SIZE];
    __local SCL_TYPE_1 Nl[SIZE][SIZE];

    const unsigned int   col = get_global_id(0);
    const unsigned int   row = get_global_id(1);
    const unsigned int l_col = get_local_id(0);
    const unsigned int l_row = get_local_id(1);
                               
    SCL_TYPE_2 sum = SCL_IDENTITY; 
    for (int m = 0; m < dimension/SIZE; ++m) {  
        Ml[l_row][l_col] = M[row * dimension + (m * SIZE + l_col)];
        Nl[l_row][l_col] = N[(m * SIZE + l_row) * width + col];
        barrier(CLK_LOCAL_MEM_FENCE);

        for (int k = 0; k < SIZE; ++k)
            sum = USR_REDUCE(sum, USR_ZIP(Ml[l_row][k], Nl[k][l_col]));
        barrier(CLK_LOCAL_MEM_FENCE);
    } 
    
    if (dimension % SIZE != 0 && SIZE > 1) {
        int m = dimension/SIZE;
        
        if (row < height && m * SIZE + l_col < dimension)
            Ml[l_row][l_col] = M[row * dimension + (m * SIZE + l_col)];
        if (col < width && m * SIZE + l_row < dimension)
            Nl[l_row][l_col] = N[(m * SIZE + l_row) * width + col];
        barrier(CLK_LOCAL_MEM_FENCE);
        
        for (int k = 0; k < dimension % SIZE; ++k)
            sum = USR_REDUCE(sum, USR_ZIP(Ml[l_row][k], Nl[k][l_col]));
        barrier(CLK_LOCAL_MEM_FENCE);
        
    }

    if (row < height && col < width)
        P[row * width + col] = sum;
}

)"
