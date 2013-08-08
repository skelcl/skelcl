int func(input_matrix_t* img, __global float* kernelVec, int range)
{
    int val = getData(img, 0, 0);
    if(val <= 20) {
        val = 0;
    } else if(val > 20 && val < 40) {
        val = 0;
    } else {
        val = 255;
    }
    return val;
}
