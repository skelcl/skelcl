int func(__local float* img, __global float* kernelVec, int range, int cols)
{
    return (getElem2D(img,0,0));
    //+getElem2DGlobal(img,0,1,cols)+getElem2DGlobal(img,-1,0,cols)+getElem2DGlobal(img,0,-1,cols))/4;
}
