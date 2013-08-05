float func(input_matrix_t* img)
{
    return (getData(img, 0, -1) + getData(img, 0, 1) + getData(img, 1, 0) + getData(img, -1, 0))/4;
}
