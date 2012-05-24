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
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include <gtest/gtest.h>

#include <iostream>
#include <iterator>
#include <fstream>
#include <string>
#include <sstream>

#include <SkelCL/detail/Program.h>
#include <SkelCL/detail/Util.h>
#include <SkelCL/detail/Logger.h>

class ProgramTest : public ::testing::Test {
protected:
  ProgramTest() {
    skelcl::detail::defaultLogger.setLoggingLevel(skelcl::detail::Logger::Severity::Debug);
  };

  ~ProgramTest() {
  };
};

TEST_F(ProgramTest, RenameFunction) {
  std::string s( "float func(float f) { return -f; }" );
  std::string hashString(skelcl::detail::util::hash(s));
  s += "\n";
  s += R"(

typedef float SCL_TYPE_0;
typedef float SCL_TYPE_1;

__kernel void SCL_MAP(
    const __global SCL_TYPE_0*  SCL_IN,
          __global SCL_TYPE_1*  SCL_OUT,
    const unsigned int          SCL_ELEMENTS )
{
  if (get_global_id(0) < SCL_ELEMENTS) {
    SCL_OUT[get_global_id(0)] = SCL_FUNC(SCL_IN[get_global_id(0)]);
  }
}
)";

  skelcl::detail::Program program(s, hashString);

  program.renameFunction("func", "SCL_FUNC");
  program.adjustTypes<int, char>();
  program.build();
}

