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

/// \cond
/// Don't show this forward declarations in doxygen
class Source;
template <typename> class Out;
template <typename> class Vector;

template<typename> class Scan;
/// \endcond

///
/// \defgroup scan Scan Skeleton
///
/// \brief The Scan skeleton describes a calculation on a Vector performed in
///        parallel on a device. Given a binary user-defined function the input
///        Vector is transformed into an out Vector by performing a scan
///        (a.k.a. prefix sum) operation.
///
/// \ingroup skeletons
///

///
/// \brief An instance of the Reduce class describes a scan (a.k.a. prefix sum)
///        calculation customized by a given binary user-defined function.
///
/// \tparam T Type of the input and output data of the skeleton.
///
/// \ingroup skeletons
/// \ingroup scan
///
template<typename T>
class Scan<T(T)> : public detail::Skeleton {
public:
  ///
  /// \brief Constructor taking the source code to customize the Scan
  ///        skeleton.
  ///
  /// \param source   Source code used to customize the skeleton.
  ///
  /// \param id       Identity for he function named by funcName and defined in
  ///                 source. Meaning: if func is the name of the function
  ///                 defined in source, func(x, id) = x and func(id, x) = x
  ///                 for every possible value of x
  ///
  /// \param funcName Name of the 'main' function (the starting point) of the
  ///                 given source code
  ///
  Scan(const Source& source,
       const std::string& id = "0",
       const std::string& funcName = std::string("func"));

  ///
  /// \brief Function call operator. Executes the skeleton on the data provided
  ///        as argument input and args. The resulting data is returned as a
  ///        moved copy.
  ///
  /// \param input The input data for the skeleton managed inside a Vector.
  ///              If no distribution is set the Single distribution using the
  ///              device with id 0 is used.
  ///              Currently only the Single distribution is supported for the
  ///              input Vector! This will certainly be advanced in a future
  ///              version.
  ///
  /// \param args  Additional arguments which are passed to the function
  ///              named by funcName and defined in the source code at created.
  ///              The individual arguments must be passed in the same order
  ///              here as they where defined in the funcName function
  ///              declaration.
  ///
  template <typename... Args>
  Vector<T> operator()(const Vector<T>& input, Args&&... args);

  ///
  /// \brief Function call operator. Executes the skeleton on the data provided
  ///        as argument input and args. The resulting data is stored in the
  ///        provided Vector output. A reference to the output Vector is
  ///        returned to allow for chaining skeleton calls.
  ///
  /// \param output The Vector storing the result of the execution of the
  ///               skeleton. A reference to this container is also returned.
  ///               The Vector might be resized to fit the result.
  ///               The distribution of the container might change.
  ///
  /// \param input The input data for the skeleton managed inside a Vector.
  ///              If no distribution is set the Single distribution using the
  ///              device with id 0 is used.
  ///              Currently only the Single distribution is supported for the
  ///              input Vector! This will certainly be advanced in a future
  ///              version.
  ///
  /// \param args  Additional arguments which are passed to the function
  ///              named by funcName and defined in the source code at created.
  ///              The individual arguments must be passed in the same order
  ///              here as they where defined in the funcName function
  ///              declaration.
  ///
  template <typename... Args>
  Vector<T>& operator()(Out<Vector<T>> output,
                        const Vector<T>& input,
                        Args&&... args);

private:
  template <typename... Args>
  void execute(Vector<T>& output,
               const Vector<T>& input,
               Args&&... args);
  
  size_t calculateNumberOfPasses(size_t workGroupSize,
                                 size_t elements) const;

  std::vector<detail::DeviceBuffer>
    createImmediateBuffers(size_t passes,
                           size_t wgSize,
                           size_t elements,
                           const detail::Device::ptr_type& devicePtr);

  void performScanPasses(size_t passes,
                         size_t wgSize,
                         const detail::Device::ptr_type& devicePtr,
                         const std::vector<detail::DeviceBuffer>& tmpBuffers,
                         const detail::DeviceBuffer& inputBuffer,
                         const detail::DeviceBuffer& outputBuffer);

  void performUniformCombination(size_t passes,
                                 size_t wgSize,
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

