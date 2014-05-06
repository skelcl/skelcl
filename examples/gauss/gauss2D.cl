int func(input_matrix_t* img, __global float* kernelVec, int range)
{
  float sum = 0.0f;
  float norm = 0.0f;
  int i, j;
  for (i = -range; i <= range; i++) {
    for (j = -range; j <= range; j++) {
      sum += getData(img, i, j) * kernelVec[(i + j) / 2 + range];
      norm += kernelVec[(i + j) / 2 + range];
    }
  }
  float v = sum / norm;
  return (v > 255) ? 255 : ((v < 0) ? 0 : v);
}

