int func(input_matrix_t* img, __global float* kernelVec, int range)
{
        float sum = 0.0;
        float norm = 0.0;
        int i,j;
        for(i=-range; i<range; i++){
                for(j=-range; j<range;j++){
                        sum += getData(img,i,j) * kernelVec[(i+j)/2+range];
                        norm += kernelVec[(i+j)/2+range];

        }
}
        return sum/norm;

    //return (2*getData(img, 1, 0) + getData(img, 1, 1) + 2*getData(img, 0, 1) + getData(img, -1, 1) + 2*getData(img, -1, 0) + getData(img, -1, -1) + 2*getData(img, 0, -1) + getData(img, 1, -1) + 4*getData(img, 0, 0))/16;

}
