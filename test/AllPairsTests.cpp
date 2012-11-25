/*****************************************************************************
 * Copyright (c) 2011-2012 The SkelCL Team as listed in CREDITS.txt          *
 * http://skelcl.uni-muenster.de                                             *
 *                                                                           *
 * This file is part of SkelCL.                                              *
 * SkelCL is available under multiple licenses.                              *
 * The different licenses are subject to terms and condition as provided     *
 * in the files specifying the license. See "LICENSE.txt" for details        *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * SkelCL is free software: you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation, either version 3 of the License, or         *
 * (at your option) any later version. See "LICENSE-gpl.txt" for details.    *
 *                                                                           *
 * SkelCL is distributed in the hope that it will be useful,                 *
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
/// \author Malte Friese <malte.friese@uni-muenster.de>
///

#include <gtest/gtest.h>

#include <cstdio>
#include <vector>

#include <SkelCL/SkelCL.h>
#include <SkelCL/Vector.h>
#include <SkelCL/AllPairs.h>
#include <SkelCL/Zip.h>
#include <SkelCL/Reduce.h>
#include <SkelCL/detail/Logger.h>

class AllPairsTest : public ::testing::Test {
protected:
    AllPairsTest() {
        //skelcl::detail::defaultLogger.setLoggingLevel(skelcl::detail::Logger::Severity::Debug);
        skelcl::init(skelcl::nDevices(1));
    }

    ~AllPairsTest() {
        skelcl::terminate();
    }
};

/*TEST_F(AllPairsTest, CreateAllPairsWithZipAndReduce) {
    skelcl::Zip<float(float, float)> zip("float func(float x, float y){ return x*y; }");
    skelcl::Reduce<float(float)> reduce("float func(float x, float y){ return x+y; }");
    skelcl::AllPairs<float(float, float)> allpairs(reduce, zip);
}*/

TEST_F(AllPairsTest, SimpleAllPairs) {
    skelcl::Zip<float(float, float)> zip("float USR_ZIP(float x, float y){ return x*y; }");
    skelcl::Reduce<float(float)> reduce("float USR_REDUCE(float x, float y){ return x+y; }");
    skelcl::AllPairs<float(float, float)> allpairs(reduce, zip);

    std::vector<float> tmpleft(4096);
    for (size_t i = 0; i < tmpleft.size(); ++i)
      tmpleft[i] = 1;//((float)i)/100;
    EXPECT_EQ(4096, tmpleft.size());

    std::vector<float> tmpright(tmpleft);
    EXPECT_EQ(4096, tmpright.size());

    skelcl::Matrix<float> left(tmpleft, 64);
    EXPECT_EQ(64, left.rowCount());
    EXPECT_EQ(64, left.columnCount());

    skelcl::Matrix<float> right(tmpright, 64);
    EXPECT_EQ(64, right.rowCount());
    EXPECT_EQ(64, right.columnCount());

    skelcl::Matrix<float> output = allpairs(left, right);
    EXPECT_EQ(64, output.rowCount());
    EXPECT_EQ(64, output.columnCount());

    int errorCount = 0;
    for (size_t i = 0; i < output.rowCount(); ++i) {
        for (size_t j = 0; j < output.columnCount(); ++j) {
            float tmp = 0;
            for (size_t k = 0; k < left.columnCount(); ++k) {
                tmp += left[i][k] * right[k][j];
            }
            if (tmp != output[i][j]) {
                //std::cout << "\\fill[color=red!30] ("<< j << "," << 63-i << ") rectangle (" << j+1 << "," << 64-i << ");" << std::endl;
                ++errorCount;
            }
            EXPECT_EQ(tmp, output[i][j]);
        }
    }
    std::cout << "Error Count: " << errorCount << std::endl;
}
