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
/// \file Out.h
///
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef OUT_H_
#define OUT_H_

#include <type_traits>

namespace skelcl {

template <typename> class Out;

///
/// \class Out
///
/// \brief This class can be used to wrap an container to be passed as
///        argument to a skeleton
///
/// When this class is used to wrap an argument which is passed to a 
/// skeleton, the data inside the wrapped container is marked as modified on
/// the device after the skeleton executed.
///
/// \tparam ContainerType The wrapped container, must be a subclass of the
///                       Container Class
///         T             The type of the data stored in the wrapped 
///                       container
///
template <template <typename> class ContainerType, typename T>
class Out< ContainerType<T> > {
public:
  ///
  /// \brief Constructor taking the container to wrap
  ///
  /// \param c Container to be wrapped
  ///
  Out< ContainerType<T> >(ContainerType<T>& c)
    : _container(c)
  {}

  ///
  /// \brief Returns a reference to the the wrapped container
  ///
  ContainerType<T>& container() const
  {
    return _container;
  }
private:
  ContainerType<T>& _container;
};

///
/// \brief Auxiliary function to create a Out wrapper object.
///
/// \param c Container to be wrapped
///
template <template <typename> class ContainerType, typename T>
Out< ContainerType<T> > out(ContainerType<T>& c)
{
  return Out< ContainerType<T> >(c);
}

} // namespace skelcl

#endif // OUT_H_
