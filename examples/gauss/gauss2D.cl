int func(__local float* img)
{
unsigned int a = getElem2D(img, -1, 1) + 2*getElem2D(img, -1, 0) + getElem2D(img, -1, -1) - getElem2D(img, 1, 1) - 2*getElem2D(img, 1, 0) - getElem2D(img, 1, -1);
unsigned int b =  getElem2D(img, -1, 1) + 2*getElem2D(img, 0, 1) + getElem2D(img, 1, 1) - getElem2D(img, -1, -1) - 2*getElem2D(img, 0, -1) - getElem2D(img, 1, -1);
float c = a*a+b*b;
 return sqrt(c);
}
