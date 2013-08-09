int func(input_matrix_t* img, __global float* kernelVec, int range)
{
    int val = getData(img, 0, 0);
    if(val <= 27 || (val > 162 && val <= 207) ) {
        if(val < getData(img, 0, 1) || val < getData(img, 0, -1)){
            return 0;
        }
        return val;
    }
    if((val > 27 && val <= 72) || (val > 207 && val <= 252)) {
        if(val < getData(img, -1, 1) || val < getData(img, 1, -1)){
            return 0;
        }
        return val;
    }
    if((val > 72 && val <= 117) || (val > 252 && val <= 297)) {
        if(val < getData(img, -1, 0) || val < getData(img, 1, 0)){
            return 0;
        }
        return val;
    }
    if((val > 117 && val <= 162) || (val > 297 && val <= 360)) {
        if(val < getData(img, -1, -1) || val < getData(img, 1, 1)){
            return 0;
        }
        return val;
    }
}
