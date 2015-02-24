int func(input_matrix_t img)
{
int neighbors = getData(img, -1, -1) + getData(img, 0, -1) + getData(img, 1, -1) + getData(img, -1, 0)
                + getData(img, 1, 0) + getData(img, -1, 1) + getData(img, 0, 1) + getData(img, 1, 1);

if(neighbors == 3 || (getData(img, 0, 0) && neighbors == 2)) {
    return 1;
}
return 0;

}

