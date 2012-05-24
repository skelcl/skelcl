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
/// \file Skeleton.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#ifndef SKELETON_H_
#define SKELETON_H_

#include "../Distribution.h"
#include "../Out.h"

#include "Container.h"

namespace skelcl {

namespace detail {

class Skeleton {
public:
  Skeleton();

  Skeleton(const unsigned workGroupSize);

  Skeleton(const Skeleton& rhs) = default;

  Skeleton& operator=(const Skeleton& rhs) = default;

  virtual ~Skeleton();

  size_t workGroupSize();

  void setWorkGroupSize(size_t size);

protected:
  template <typename... Args>
  void prepareAdditionalInput(Args&&... args);

  void prepareAdditionalInput();

  template <typename T, template <typename> class ContainerType, typename... Args>
  void prepareAdditionalInput(Out<ContainerType<T>> outContainer, Args&&... args);

  template <typename T, template <typename> class ContainerType, typename... Args>
  void prepareAdditionalInput(ContainerType<T>& container, Args&&... args);

  template <typename T, typename... Args>
  void prepareAdditionalInput(T t, Args&&... args);

  template <typename... Args>
  void updateModifiedStatus(Args&&... args);

  void updateModifiedStatus();

  template <typename T, template <typename> class ContainerType, typename... Args>
  void updateModifiedStatus(Out<ContainerType<T>> outVector, Args&&... args);

  template <typename T, typename... Args>
  void updateModifiedStatus(T&& t, Args&&... args);

private:
  unsigned _workGroupSize;
};

void Skeleton::prepareAdditionalInput()
{
}

template <typename T, template <typename> class ContainerType, typename... Args>
void Skeleton::prepareAdditionalInput(Out<ContainerType<T>> outContainer,
                                      Args&&... args)
{
  prepareAdditionalInput( outContainer.container(), std::forward<Args>(args)... );
}

template <typename T, template <typename> class ContainerType, typename... Args>
void Skeleton::prepareAdditionalInput(ContainerType<T>& container,
                                      Args&&... args)
{
  static_assert(std::is_base_of<skelcl::detail::Container<T>,
                               ContainerType<T>>::value,
      "Argument has to be derived class of skelcl::detail::Container<T>");

  // set default distribution if required
  if (container.distribution() == nullptr) {
    container.setDistribution(Distribution::Copy());
  }
  // create buffers if required
  container.createDeviceBuffers();
  // copy data to devices
  container.startUpload();

  prepareAdditionalInput( std::forward<Args>(args)... );
}

template <typename T, typename... Args>
void Skeleton::prepareAdditionalInput(T /*t*/, Args&&... args)
{
  // skip argument and call prepareInputs on next one
  prepareAdditionalInput( std::forward<Args>(args)... );
}


void Skeleton::updateModifiedStatus()
{
}

template <typename T, template <typename> class ContainerType, typename... Args>
void Skeleton::updateModifiedStatus(Out<ContainerType<T>> outContainer, Args&&... args)
{
  outContainer.container().dataOnDeviceModified();
  updateModifiedStatus( std::forward<Args>(args)... );
}

template <typename T, typename... Args>
void Skeleton::updateModifiedStatus(T&& /*t*/, Args&&... args)
{
  // skip argument and call updateModifiedStatus on next one
  updateModifiedStatus( std::forward<Args>(args)... );
}

} // namespace detail

} // namespace skelcl

#endif // SKELETON_H_

