float func(input_matrix_t* img, __global float* kernelVec, int range, int range2)
{
//return getData(img, 0, 10);
        float sum = 0.0;
        float norm = 0.0;
        int i,j;
        for(i=-range; i<range; i++){
                for(j=-range2; j<range;j++){
                        sum += getData(img,i,j) * kernelVec[(i+j)/2+range];
                        norm += kernelVec[(i+j)/2+range];

        }
}
        return sum/norm;

}
