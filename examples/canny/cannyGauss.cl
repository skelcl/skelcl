float func(input_matrix_t img, __global float* kernelVec, int range)
{
        float sum = 0.0;
        float norm = 0.0;
        int i,j;
        for(i=-range; i<=range; i++){
                for(j=-range; j<=range;j++){
                        sum += getData(img,i,j) * kernelVec[(i+j)/2+range];
                        norm += kernelVec[(i+j)/2+range];

        }
}
        return sum/norm;
}
