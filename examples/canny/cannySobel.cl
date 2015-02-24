float func(input_matrix_t img, __global float* kernelVec, int range)
{
   int a = getData(img, -1, 1) + 2*getData(img, -1, 0) + getData(img, -1, -1) - getData(img, 1, 1) - 2*getData(img, 1, 0) - getData(img, 1, -1);
   int b =  getData(img, -1, 1) + 2*getData(img, 0, 1) + getData(img, 1, 1) - getData(img, -1, -1) - 2*getData(img, 0, -1) - getData(img, 1, -1);
   float c = a*a+b*b;
   return sqrt(c);
}
