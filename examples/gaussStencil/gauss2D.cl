int func(input_matrix_t* img, __global float* kernelVec, int rangeNorth, int rangeWest, int rangeSouth, int rangeEast)
{
float sum = 0.0;
float norm = 0.0;
int i,j;
for(i=-rangeWest; i<=rangeEast; i++){
        for(j=-rangeSouth; j<=rangeNorth;j++){
                sum += getData(img,i,j) * kernelVec[(i+j)/2+rangeNorth];
                norm += kernelVec[(i+j)/2+rangeNorth];

}
}
return sum/norm;

}
