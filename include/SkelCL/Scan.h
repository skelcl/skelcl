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
/// \file Scan.h
///
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef SCAN_H_
#define SCAN_H_

#include <istream>
#include <string>

#include "detail/Skeleton.h"
#include "detail/Program.h"

namespace skelcl {

class Source;
template <typename> class Out;
template <typename> class Vector;

template<typename> class Scan;

template<typename T>
class Scan<T(T)> : public detail::Skeleton {
public:
  Scan(const Source& source,
       const std::string& id = "0",
       const std::string& funcName = std::string("func"));

  template <typename... Args>
  Vector<T> operator()(const Vector<T>& input, Args&&... args);

  template <typename... Args>
  Vector<T>& operator()(Out<Vector<T>> output,
                        const Vector<T>& input,
                        Args&&... args);

private:
  template <typename... Args>
  void execute(Vector<T>& output,
               const Vector<T>& input,
               Args&&... args);
  
  unsigned int calculateNumberOfPasses(size_t workGroupSize,
                                       size_t elements) const;

  std::vector<detail::DeviceBuffer>
    createImmediateBuffers(unsigned int passes,
                           unsigned int wgSize,
                           unsigned int elements,
                           const detail::Device::ptr_type& devicePtr);

  void performScanPasses(unsigned int passes,
                         unsigned int wgSize,
                         const detail::Device::ptr_type& devicePtr,
                         const std::vector<detail::DeviceBuffer>& tmpBuffers,
                         const detail::DeviceBuffer& inputBuffer,
                         const detail::DeviceBuffer& outputBuffer);

  void performUniformCombination(unsigned int passes,
                                 unsigned int wgSize,
                                 const detail::Device::ptr_type& devicePtr,
                                 const std::vector<detail::DeviceBuffer>&
                                    tmpBuffers,
                                 const detail::DeviceBuffer& outputBuffer);

  void prepareInput(const Vector<T>& input);

  void prepareOutput(Vector<T>& output,
                     const Vector<T>& input);
  
  detail::Program createAndBuildProgram(const std::string& source,
                                        const std::string& id,
                                        const std::string& funcName) const;

  const detail::Program _program;
};

} // namespace skelcl

#include "detail/ScanDef.h"

#endif // Scan_H_

