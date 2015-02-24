float func(input_matrix_t img)
{
float t = getData(img, 0, 1);
float b = getData(img, 0, -1);
float l = getData(img, -1, 0);
float r = getData(img, 1, 0);
float c = getData(img, 0, 0);

if(c==255.0) return 255.0;

return c + (t+b+r+l-4*c)/4;

}
