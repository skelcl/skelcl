/*****************************************************************************
 * Copyright (c) 2011-2012 The skelcl Team as listed in CREDITS.txt          *
 * http://skelcl.uni-muenster.de                                             *
 *                                                                           *
 * This file is part of skelcl.                                              *
 * skelcl is available under multiple licenses.                              *
 * The different licenses are subject to terms and condition as provided     *
 * in the files specifying the license. See "LICENSE.txt" for details        *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * skelcl is free software: you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation, either version 3 of the License, or         *
 * (at your option) any later version. See "LICENSE-gpl.txt" for details.    *
 *                                                                           *
 * skelcl is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
 * GNU General Public License for more details.                              *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * For non-commercial academic use see the license specified in the file     *
 * "LICENSE-academic.txt".                                                   *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * If you are interested in other licensing models, including a commercial-  *
 * license, please contact the author at michel.steuwer@uni-muenster.de      *
 *                                                                           *
 *****************************************************************************/

///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include <fstream>
#include <cstdio>

#include <pvsutil/Logger.h>

#include <SkelCL/SkelCL.h>
#include <SkelCL/Matrix.h>
#include <SkelCL/Vector.h>
#include <SkelCL/MapOverlap.h>

#include "Test.h"

#include <iostream>

class MapOverlapTest : public ::testing::Test {
protected:
  MapOverlapTest() {
    //pvsutil::defaultLogger.setLoggingLevel(pvsutil::Logger::Severity::DebugInfo);
    skelcl::init(skelcl::nDevices(1).deviceType(skelcl::device_type::CPU));
  };

  ~MapOverlapTest() {
    skelcl::terminate();
  };
};

TEST_F(MapOverlapTest, CreateMapOverlapWithString) {
  skelcl::MapOverlap<float(float)> m {"float func(input_matrix_t f) \
    { return -getData(f, 0, 0); }", 1};
}

#if 0
TEST_F(MapOverlapTest, SimpleMapOverlap) {
  skelcl::MapOverlap<int(int)> m{
    "int func(__local int* f){ return f[0]; }", 1 };

  skelcl::Vector<int> input(10);
  for (size_t i = 0; i < input.size(); ++i) {
    input[i] = i;
  }
  EXPECT_EQ(10, input.size());

  skelcl::Vector<int> output = m(input);

  EXPECT_EQ(10, output.size());
  for (size_t i = 0; i < output.size(); ++i) {
    EXPECT_EQ(input[i], output[i]);
  }
}

TEST_F(MapOverlapTest, SimpleMapOverlap2) {
  skelcl::MapOverlap<int(int)> m{
    "int func(__local int* f){ return f[-1]+f[0]+f[1]; }", 1 };

  skelcl::Vector<int> input(10);
  for (size_t i = 0; i < input.size(); ++i) {
    input[i] = i;
  }
  EXPECT_EQ(10, input.size());

  skelcl::Vector<int> output = m(input);

  EXPECT_EQ(10, output.size());
  EXPECT_EQ(0+input[0]+input[1], output[0]);
  for (size_t i = 1; i < output.size() - 1; ++i) {
    EXPECT_EQ(input[i-1]+input[i]+input[i+1], output[i]);
  }
  EXPECT_EQ(input[input.size()-2]+input[input.size()-1]+0, output.back());
}

TEST_F(MapOverlapTest, SimpleMapOverlapWithZeroOverlap) {
  skelcl::MapOverlap<float(float)> m{
    "float func(__local float* f){ return -f[0]; }", 0 };

  skelcl::Vector<float> input(10);
  for (size_t i = 0; i < input.size(); ++i) {
    input[i] = i * 2.5f;
  }
  EXPECT_EQ(10, input.size());

  skelcl::Vector<float> output = m(input);

  EXPECT_EQ(10, output.size());
  for (size_t i = 0; i < output.size(); ++i) {
    EXPECT_EQ(-input[i], output[i]);
  }
}

TEST_F(MapOverlapTest, SimpleMultiDeviceMapOverlap) {
  skelcl::terminate();
  skelcl::init(skelcl::nDevices(2));
  skelcl::MapOverlap<int(int)> m{
    "int func(__local int* f){ return f[0]; }", 1 };

  skelcl::Vector<int> input(10);
  for (size_t i = 0; i < input.size(); ++i) {
    input[i] = i;
  }
  EXPECT_EQ(10, input.size());

  skelcl::Vector<int> output = m(input);

  EXPECT_EQ(10, output.size());
  for (size_t i = 0; i < output.size(); ++i) {
    EXPECT_EQ(input[i], output[i]);
  }
}

TEST_F(MapOverlapTest, SimpleMultiDeviceMapOverlap2) {
  skelcl::terminate();
  skelcl::init(skelcl::nDevices(2));
  skelcl::MapOverlap<int(int)> m{
    "int func(__local int* f){ return f[-1] + f[0] + f[+1]; }", 1 };

  skelcl::Vector<int> input(499);
  for (size_t i = 0; i < input.size(); ++i) {
    input[i] = i;
  }
  EXPECT_EQ(499, input.size());

  skelcl::Vector<int> output = m(input);

  EXPECT_EQ(499, output.size());
  EXPECT_EQ(0+input[0]+input[1], output[0]);
  for (size_t i = 1; i < output.size() - 1; ++i) {
    EXPECT_EQ(input[i-1]+input[i]+input[i+1], output[i]);
  }
  EXPECT_EQ(input[input.size()-2]+input[input.size()-1]+0, output.back());
}
#endif


const auto print = [](skelcl::Matrix<int>& m, std::string name) {
  std::cout << name << "\n";
  auto i = 0u;
  for (auto& v : m) {
    ++i;
    std::cout << std::setw(2) << v << " ";
    if (i == m.columnCount()) {
      i = 0;
      std::cout << "\n";
    }
  }
};

TEST_F(MapOverlapTest, SimpleMatrixRightShift) {
  skelcl::MapOverlap<int(int)> m{
      "int func(input_matrix_t f){ return getData(f, -1, 0); }", 1};

  size_t size = 1024;

  skelcl::Matrix<int> input( skelcl::MatrixSize{size, size} );
  for (size_t i = 0; i < input.size().rowCount(); ++i) { // y direction
    for (size_t j = 0; j < input.size().columnCount(); ++j) { // x direction
      input[i][j] = (i*j)+i;
    }
  }

  skelcl::Matrix<int> output = m(input);

  EXPECT_EQ(size, output.size().rowCount());
  EXPECT_EQ(size, output.size().columnCount());

  for (size_t i = 0; i < output.size().rowCount(); ++i) {
    EXPECT_EQ(input[i][0], output[i][0]);
  }

  for (size_t i = 0; i < output.size().rowCount(); ++i) {
    for (size_t j = 1; j < output.size().columnCount(); ++j) {
      EXPECT_EQ(input[i][j-1], output[i][j]);
    }
  }
  //print(input, "input");
  //print(output, "output");
}

TEST_F(MapOverlapTest, SimpleMatrixTwoRightShift) {
  auto size = 100u;
  auto shift = 2u;
  skelcl::MapOverlap<int(int)> m{
      "int func(input_matrix_t f){ return getData(f, -2, 0); }", shift};

  skelcl::Matrix<int> input( skelcl::MatrixSize{size, size} );
  for (size_t i = 0; i < input.size().rowCount(); ++i) {
    for (size_t j = 0; j < input.size().columnCount(); ++j) {
      input[i][j] = j;
    }
  }

  skelcl::Matrix<int> output = m(input);

  EXPECT_EQ(size, output.size().rowCount());
  EXPECT_EQ(size, output.size().columnCount());

  for (size_t i = 0; i < output.size().rowCount(); ++i) {
    for (size_t j = shift; j < output.size().columnCount(); ++j) {
      EXPECT_EQ(input[i][j - shift], output[i][j]);
    }
  }

  for (size_t i = 0; i < output.size().rowCount(); ++i) {
    for (size_t j = 0; j < shift; ++j) {
      EXPECT_EQ(input[i][0], output[i][j]);
    }
  }
  //print(input, "input");
  //print(output, "output");
}

TEST_F(MapOverlapTest, SimpleMatrixLeftShift) {
  skelcl::MapOverlap<int(int)> m{
      "int func(input_matrix_t f){ return getData(f, +1, 0); }", 1};

  size_t size = 100;

  skelcl::Matrix<int> input( skelcl::MatrixSize{size, size} );
  for (size_t i = 0; i < input.size().rowCount(); ++i) {
    for (size_t j = 0; j < input.size().columnCount(); ++j) {
      input[i][j] = j;
    }
  }

  skelcl::Matrix<int> output = m(input);

  EXPECT_EQ(size, output.size().rowCount());
  EXPECT_EQ(size, output.size().columnCount());

  for (size_t i = 0; i < output.size().rowCount(); ++i) {
    for (size_t j = 0; j < output.size().columnCount() - 1; ++j) {
      EXPECT_EQ(input[i][j+1], output[i][j]);
    }
  }

  for (size_t i = 0; i < output.size().rowCount(); ++i) {
    size_t j = output.size().columnCount() - 1;
    EXPECT_EQ(input[i][j], output[i][j]);
  }
  //print(input, "input");
  //print(output, "output");
}

TEST_F(MapOverlapTest, SimpleMatrixTwoLeftShift) {
  auto size = 100u;
  auto shift = 2u;
  skelcl::MapOverlap<int(int)> m{
      "int func(input_matrix_t f){ return getData(f, +2, 0); }", shift};

  skelcl::Matrix<int> input( skelcl::MatrixSize{size, size} );
  for (size_t i = 0; i < input.size().rowCount(); ++i) {
    for (size_t j = 0; j < input.size().columnCount(); ++j) {
      input[i][j] = j;
    }
  }

  skelcl::Matrix<int> output = m(input);

  EXPECT_EQ(size, output.size().rowCount());
  EXPECT_EQ(size, output.size().columnCount());

  for (size_t i = 0; i < output.size().rowCount(); ++i) {
    for (size_t j = 0; j < output.size().columnCount() - shift; ++j) {
      EXPECT_EQ(input[i][j + shift], output[i][j]);
    }
  }

  for (size_t i = 0; i < output.size().rowCount(); ++i) {
    for (size_t j = output.size().columnCount() - shift;
         j < output.size().columnCount(); ++j) {
      EXPECT_EQ(input[i][output.size().columnCount()-1], output[i][j]);
    }
  }
  //print(input, "input");
  //print(output, "output");
}

TEST_F(MapOverlapTest, SimpleMatrixDownShift) {
  skelcl::MapOverlap<int(int)> m{
      "int func(input_matrix_t f){ return getData(f, 0, -1); }", 1};

  size_t size = 1024;

  skelcl::Matrix<int> input( skelcl::MatrixSize{size, size} );
  for (size_t i = 0; i < input.size().rowCount(); ++i) {
    for (size_t j = 0; j < input.size().columnCount(); ++j) {
      input[i][j] = i;
    }
  }

  skelcl::Matrix<int> output = m(input);

  EXPECT_EQ(size, output.size().rowCount());
  EXPECT_EQ(size, output.size().columnCount());

  for (size_t j = 0; j < output.size().columnCount(); ++j) {
    EXPECT_EQ(input[0][j], output[0][j]);
  }

  for (size_t i = 1; i < output.size().rowCount(); ++i) {
    for (size_t j = 0; j < output.size().columnCount(); ++j) {
      EXPECT_EQ(input[i-1][j], output[i][j]);
    }
  }
  //print(input, "input");
  //print(output, "output");
}

TEST_F(MapOverlapTest, SimpleMatrixTwoDownShift) {
  auto size = 100u;
  auto shift = 2u;
  skelcl::MapOverlap<int(int)> m{
      "int func(input_matrix_t f){ return getData(f, 0, -2); }", shift};

  skelcl::Matrix<int> input( skelcl::MatrixSize{size, size} );
  for (size_t i = 0; i < input.size().rowCount(); ++i) {
    for (size_t j = 0; j < input.size().columnCount(); ++j) {
      input[i][j] = i;
    }
  }

  skelcl::Matrix<int> output = m(input);

  EXPECT_EQ(size, output.size().rowCount());
  EXPECT_EQ(size, output.size().columnCount());

  for (size_t i = 0; i < shift; i++) {
    for (size_t j = 0; j < output.size().columnCount(); ++j) {
      EXPECT_EQ(input[0][j], output[i][j]);
    }
  }

  for (size_t i = shift; i < output.size().rowCount(); ++i) {
    for (size_t j = 0; j < output.size().columnCount(); ++j) {
      EXPECT_EQ(input[i-shift][j], output[i][j]);
    }
  }
  //print(input, "input");
  //print(output, "output");
}

TEST_F(MapOverlapTest, SimpleMatrixUpShift) {
  auto size = 1024u;
  auto shift = 1u;
  skelcl::MapOverlap<int(int)> m{
      "int func(input_matrix_t f){ return getData(f, 0, +1); }", shift};

  skelcl::Matrix<int> input( skelcl::MatrixSize{size, size} );
  for (size_t i = 0; i < input.size().rowCount(); ++i) {
    for (size_t j = 0; j < input.size().columnCount(); ++j) {
      input[i][j] = i;
    }
  }

  skelcl::Matrix<int> output = m(input);

  EXPECT_EQ(size, output.size().rowCount());
  EXPECT_EQ(size, output.size().columnCount());

  for (size_t i = 0; i < output.size().rowCount() - shift; ++i) {
    for (size_t j = 0; j < output.size().columnCount(); ++j) {
      EXPECT_EQ(input[i + shift][j], output[i][j]);
    }
  }

  for (size_t i = output.size().rowCount() - shift;
       i < output.size().rowCount(); i++) {
    for (size_t j = 0; j < output.size().columnCount(); ++j) {
      EXPECT_EQ(input[i][j], output[i][j]);
    }
  }
  //print(input, "input");
  //print(output, "output");
}

TEST_F(MapOverlapTest, SimpleMatrixTwoUpShift) {
  auto size = 100u;
  auto shift = 2u;
  skelcl::MapOverlap<int(int)> m{
      "int func(input_matrix_t f){ return getData(f, 0, +2); }", shift};

  skelcl::Matrix<int> input( skelcl::MatrixSize{size, size} );
  for (size_t i = 0; i < input.size().rowCount(); ++i) {
    for (size_t j = 0; j < input.size().columnCount(); ++j) {
      input[i][j] = i;
    }
  }

  skelcl::Matrix<int> output = m(input);

  EXPECT_EQ(size, output.size().rowCount());
  EXPECT_EQ(size, output.size().columnCount());

  for (size_t i = 0; i < output.size().rowCount() - shift; ++i) {
    for (size_t j = 0; j < output.size().columnCount(); ++j) {
      EXPECT_EQ(input[i + shift][j], output[i][j]);
    }
  }

  for (size_t i = output.size().rowCount() - shift;
       i < output.size().rowCount(); i++) {
    for (size_t j = 0; j < output.size().columnCount(); ++j) {
      EXPECT_EQ(input[output.size().rowCount()-1][j], output[i][j]);
    }
  }
  //print(input, "input");
  //print(output, "output");
}

#if 0
TEST_F(MapOverlapTest, SimpleMatrixMapOverlap2) {
  skelcl::MapOverlap<int(int)> m{
    "int func(__local int* f){ return f[-1]+f[0]+f[1]; }", 1 };

  skelcl::Vector<int> input(10);
  for (size_t i = 0; i < input.size(); ++i) {
    input[i] = i;
  }
  EXPECT_EQ(10, input.size());

  skelcl::Vector<int> output = m(input);

  EXPECT_EQ(10, output.size());
  EXPECT_EQ(0+input[0]+input[1], output[0]);
  for (size_t i = 1; i < output.size() - 1; ++i) {
    EXPECT_EQ(input[i-1]+input[i]+input[i+1], output[i]);
  }
  EXPECT_EQ(input[input.size()-2]+input[input.size()-1]+0, output.back());
}

TEST_F(MapOverlapTest, SimpleMatrixMultiDeviceMapOverlap) {
  skelcl::terminate();
  skelcl::init(2);
  skelcl::MapOverlap<int(int)> m{
    "int func(__local int* f){ return f[0]; }", 1 };

  skelcl::Vector<int> input(10);
  for (size_t i = 0; i < input.size(); ++i) {
    input[i] = i;
  }
  EXPECT_EQ(10, input.size());

  skelcl::Vector<int> output = m(input);

  EXPECT_EQ(10, output.size());
  for (size_t i = 0; i < output.size(); ++i) {
    EXPECT_EQ(input[i], output[i]);
  }
}

TEST_F(MapOverlapTest, SimpleMatrixMultiDeviceMapOverlap2) {
  skelcl::terminate();
  skelcl::init(2);
  skelcl::MapOverlap<int(int)> m{
    "int func(__local int* f){ return f[-1] + f[0] + f[+1]; }", 1 };

  skelcl::Vector<int> input(499);
  for (size_t i = 0; i < input.size(); ++i) {
    input[i] = i;
  }
  EXPECT_EQ(499, input.size());

  skelcl::Vector<int> output = m(input);

  EXPECT_EQ(499, output.size());
  EXPECT_EQ(0+input[0]+input[1], output[0]);
  for (size_t i = 1; i < output.size() - 1; ++i) {
    EXPECT_EQ(input[i-1]+input[i]+input[i+1], output[i]);
  }
  EXPECT_EQ(input[input.size()-2]+input[input.size()-1]+0, output.back());
}
#endif
