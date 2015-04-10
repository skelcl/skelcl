#ifndef _COMMON_H_
#define _COMMON_H_

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <cstdlib>

#include <pvsutil/CLArgParser.h>
#include <pvsutil/Logger.h>

#include <SkelCL/SkelCL.h>
#include <SkelCL/Stencil.h>

using namespace skelcl;

template<typename T>
void printMatrix(Matrix<T> &m) {
  const size_t max = 5;
  const size_t rows = std::min(max, m.rowCount());
  const size_t cols = std::min(max, m.columnCount());

  for (size_t j = 0; j < rows; j++) {
    for (size_t i = 0; i < cols; i++)
      std::cout << m[j][i] << " ";
    if (cols < m.columnCount())
      std::cout << " ...";
    std::cout << "\n";
  }
  if (rows < m.rowCount())
    std::cout << "...\n";
}

#endif  // _COMMON_H_
