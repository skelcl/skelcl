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
/// \file Map.cpp
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include <string>

#include <pvsutil/Assert.h>

#include "SkelCL/Map.h"
#include "SkelCL/Source.h"

#include "SkelCL/detail/Program.h"

namespace skelcl {
  
  // ## Map<Index, void> ################################################
  
  Map<void(Index)>::Map(const Source& source,
                        const std::string& funcName)
  : Skeleton(),
  detail::MapHelper<void(Index)>(createAndBuildProgram(source, funcName))
  {
  }
  
  detail::Program Map<void(Index)>::createAndBuildProgram(const std::string& source,
                                                          const std::string& funcName) const
  {
    ASSERT_MESSAGE(!source.empty(),
                   "Tried to create program with empty user source.");
    
    // create program
    // first: device specific functions
    std::string s(detail::CommonDefinitions::getSource());
    s.append(R"(
             typedef size_t Index;
             
             )");
    // second: user defined source
    s.append(source);
    // last: append skeleton implementation source
    s.append(R"(
             
             __kernel void SCL_MAP(const unsigned int SCL_OFFSET)
    {
      SCL_FUNC(get_global_id(0)+SCL_OFFSET);
    }
             )");
    auto program = detail::Program(s, detail::util::hash(s));
    
    // modify program
    if (!program.loadBinary()) {
      // append parameters from user function to kernel
      program.transferParameters(funcName, 1, "SCL_MAP");
      program.transferArguments(funcName, 1, "SCL_FUNC");
      // rename user function
      program.renameFunction(funcName, "SCL_FUNC");
    }
    
    // build program
    program.build();
    
    return program;
  }
  
  
  // ## Map<IndexPoint, void> ################################################
  Map<void(IndexPoint)>::Map(const Source& source,
                             const std::string& funcName)
  : Skeleton(),
  detail::MapHelper<void(IndexPoint)>(createAndBuildProgram(source, funcName))
  {
  }
  
  detail::Program Map<void(IndexPoint)>::createAndBuildProgram(const std::string& source,
                                                               const std::string& funcName) const
  {
    ASSERT_MESSAGE(!source.empty(),
                   "Tried to create program with empty user source.");
    
    // create program
    // first: device specific functions
    std::string s(detail::CommonDefinitions::getSource());
    s.append(R"(
             typedef struct {
               size_t x;
               size_t y;
             } IndexPoint;
             
             )");
    // second: user defined source
    s.append(source);
    // last: append skeleton implementation source
    s.append(R"(
             
             __kernel void SCL_MAP(const unsigned int SCL_ROW_OFFSET)
             {
               // dim 1 is the columns, dim 0 the rows
               IndexPoint p;
               p.x = get_global_id(1);
               p.y = get_global_id(0) + SCL_ROW_OFFSET;
               SCL_FUNC(p);
             }
             )");
    auto program = detail::Program(s, detail::util::hash(s));
    
    // modify program
    if (!program.loadBinary()) {
      // append parameters from user function to kernel
      program.transferParameters(funcName, 1, "SCL_MAP");
      program.transferArguments(funcName, 1, "SCL_FUNC");
      // rename user function
      program.renameFunction(funcName, "SCL_FUNC");
    }
    
    // build program
    program.build();
    
    return program;
  }
  
} // namespace skelcl
