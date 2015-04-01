float func(input_matrix_t img)
{
  float centre = getData(img,  0,  0);

  // Neighbouring elements.
  float top    = getData(img,  0,  1);
  float bottom = getData(img,  0, -1);
  float left   = getData(img, -1,  0);
  float right  = getData(img,  1,  0);

  // Sum neighbours
  float neighbours = top + bottom + left + right;

  // Diffusion.
  return centre + (neighbours - 4 * centre) / 4;
}
