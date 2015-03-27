float func(input_matrix_t* img, __global float* kernelVec, int range)
{
    float val;

    // GAUSSIAN
    float sum = 0.0;
    float norm = 0.0;
    int i,j;
    for(i=-range; i<=range; i++){
        for(j=-range; j<=range;j++){
            sum += getData(img,i,j) * kernelVec[(i+j)/2+range];
            norm += kernelVec[(i+j)/2+range];

        }
    }
    float ret1 = sum/norm;

    barrier(CLK_GLOBAL_MEM_FENCE);

    // SOBEL
    int a = getData(img, -1, 1) + 2*getData(img, -1, 0)
            + getData(img, -1, -1) - getData(img, 1, 1)
            - 2*getData(img, 1, 0) - getData(img, 1, -1);
    int b =  getData(img, -1, 1) + 2*getData(img, 0, 1)
            + getData(img, 1, 1) - getData(img, -1, -1)
            - 2*getData(img, 0, -1) - getData(img, 1, -1);
    float c = a*a+b*b;
    float ret2 = sqrt(c);

    barrier(CLK_GLOBAL_MEM_FENCE);

    // NMS
    val = ret2;
    float ret3;
    if(val <= 27 || (val > 162 && val <= 207) ) {
        if(val < getData(img, 0, 1) || val < getData(img, 0, -1)){
            ret3 = 0;
        }
        ret3 = val;
    }
    if((val > 27 && val <= 72) || (val > 207 && val <= 252)) {
        if(val < getData(img, -1, 1) || val < getData(img, 1, -1)){
            ret3 = 0;
        }
        ret3 = val;
    }
    if((val > 72 && val <= 117) || (val > 252 && val <= 297)) {
        if(val < getData(img, -1, 0) || val < getData(img, 1, 0)){
            ret3 = 0;
        }
        ret3 = val;
    }
    if((val > 117 && val <= 162) || (val > 297 && val <= 360)) {
        if(val < getData(img, -1, -1) || val < getData(img, 1, 1)){
            ret3 = 0;
        }
        ret3 = val;
    }

    barrier(CLK_GLOBAL_MEM_FENCE);

    // Threshold
    val = ret3;

    float ret4;
    if(val <= 20) {
        ret4 = 0;
    } else if(val > 20 && val < 40) {
        ret4 = 0;
    } else {
        ret4 = 255;
    }

    return ret4;
}
