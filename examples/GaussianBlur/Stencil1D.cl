float func(input_matrix_t* img, __global float* kernelVec, int range) {
  float sum = 0.0;
  float norm = 0.0;
  int i;

  for(i = -range; i <= range; i++) {
    sum += getData(img, 0, i) * kernelVec[i + range];
    norm += kernelVec[i + range];
  }

  return sum / norm;
}
