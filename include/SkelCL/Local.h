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
/// \file Local.h
///
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef LOCAL_H_
#define LOCAL_H_

#include <cstring>

#include "detail/skelclDll.h"

namespace skelcl {

///
/// \brief This class represents OpenCL local memory in SkelCL.
///
/// An object of this class can be used as an additional argument of a skeleton
/// to make local memory available in the user defined function.
///
/// The helper function local(size_t) can be used to create an object of this
/// class directly when the skeleton is executed.
///
class SKELCL_DLL Local {
public:
  ///
  /// \brief Create a new Local object with the given size of local memory in
  ///        bytes.
  ///
  /// \param sizeInBytes The size of local memory to be used in bytes.
  ///
  Local(size_t sizeInBytes);

  ///
  /// \brief Access the amount of local memory to be used in bytes.
  ///
  /// \return The amount of local memory to be used in bytes.
  ///
  size_t getSizeInBytes() const;

private:
  size_t _sizeInBytes;
};

///
/// \brief Helper function to easily create an Local object with a given size.
///
/// \param sizeInBytes The amount of local memory to be used in bytes.
///
/// \return A newly created Local object representing the given amount of local
///         memory.
///
SKELCL_DLL Local local(size_t sizeInBytes);

} // namespace skelcl

#endif // LOCAL_H_
