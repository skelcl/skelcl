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

#include "../Distributions.h"
#include "../Out.h"

#include "skelclDll.h"

namespace skelcl {

namespace detail {

class SKELCL_DLL Skeleton {
public:
  Skeleton(const std::string name);

  Skeleton(const std::string name, const unsigned workGroupSize);

  Skeleton(const Skeleton& rhs) = default;

  Skeleton& operator=(const Skeleton& rhs) = default;

  virtual ~Skeleton();

  size_t workGroupSize() const;

  void setWorkGroupSize(size_t size);

protected:
  template <typename... Args>
  void prepareAdditionalInput(Args&&... args) const;

  void prepareAdditionalInput() const;

  template <typename T, template <typename> class C, typename... Args>
  void prepareAdditionalInput(Out<C<T>> outContainer,
                              Args&&... args) const;

  template <typename T, template <typename> class C, typename... Args>
  void prepareAdditionalInput(C<T>& container, Args&&... args) const;

  template <typename T, typename... Args>
  void prepareAdditionalInput(T t, Args&&... args) const;

  template <typename... Args>
  void updateModifiedStatus(Args&&... args) const;

  void updateModifiedStatus() const;

  template <typename T, template <typename> class C, typename... Args>
  void updateModifiedStatus(Out<C<T>> outVector, Args&&... args) const;

  template <typename T, typename... Args>
  void updateModifiedStatus(T&& t, Args&&... args) const;

  // The name of the skeleton, used in debugging. Can be set to any
  // arbitrary string.
  const std::string _name;

  std::vector<cl::Event> _events;

  size_t _workGroupSize;

private:
  
  void printEventTimings() const;
};

template <typename T, template <typename> class C, typename... Args>
void Skeleton::prepareAdditionalInput(Out<C<T>> outContainer,
                                      Args&&... args) const
{
  prepareAdditionalInput(outContainer.container(), std::forward<Args>(args)...);
}

template <typename T, template <typename> class C, typename... Args>
void Skeleton::prepareAdditionalInput(C<T>& container,
                                      Args&&... args) const
{
  // set default distribution if required
  if (!container.distribution().isValid()) {
    container.setDistribution(CopyDistribution<C<T>>());
  }
  // create buffers if required
  container.createDeviceBuffers();
  // copy data to devices
  container.startUpload();

  prepareAdditionalInput( std::forward<Args>(args)... );
}

template <typename T, typename... Args>
void Skeleton::prepareAdditionalInput(T /*t*/, Args&&... args) const
{
  // skip argument and call prepareInputs on next one
  prepareAdditionalInput( std::forward<Args>(args)... );
}

template <typename T, template <typename> class C, typename... Args>
void Skeleton::updateModifiedStatus(Out<C<T>> outContainer,
                                    Args&&... args) const
{
  outContainer.container().dataOnDeviceModified();
  updateModifiedStatus( std::forward<Args>(args)... );
}

template <typename T, typename... Args>
void Skeleton::updateModifiedStatus(T&& /*t*/, Args&&... args) const
{
  // skip argument and call updateModifiedStatus on next one
  updateModifiedStatus( std::forward<Args>(args)... );
}

} // namespace detail

} // namespace skelcl

#endif // SKELETON_H_

