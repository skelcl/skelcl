int func(input_matrix_t* img, __global float* kernelVec, int rangeNorth, int rangeWest, int rangeSouth, int rangeEast)
{
int range = rangeNorth;
range = rangeSouth > range ? rangeSouth : range;
range = rangeWest > range ? rangeWest : range;
range = rangeEast > range ? rangeEast : range;

float sum = 0.0;
float norm = 0.0;
int i,j;
for(i=-rangeWest; i<rangeEast; i++){
        for(j=-rangeSouth; j<rangeNorth;j++){
                sum += getData(img,i,j) * kernelVec[(i+j)/2+range];
                norm += kernelVec[(i+j)/2+range];

}
}
return sum/norm;

}
