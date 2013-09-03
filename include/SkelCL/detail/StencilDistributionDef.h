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
/// \file StencilDistributionDef.h
///
///
///     \author Stefan Breuer <s_breu03@uni-muenster.de>
///
#ifndef STENCILDISTRIBUTION_DEF_H_
#define STENCILDISTRIBUTION_DEF_H_

namespace skelcl {

namespace detail {

template<template<typename > class C, typename T>
StencilDistribution<C<T>>::StencilDistribution(const unsigned int north,
        const unsigned int west, const unsigned int south,
        const unsigned int east, const detail::Padding padding,
        const T neutral_element, const unsigned int initialIterationsBeforeFirstSwap, const DeviceList& deviceList) :
        _north(north), _west(west), _south(south), _east(east), _padding(
        padding), _neutral_element(neutral_element), _initialIterationsBeforeFirstSwap(initialIterationsBeforeFirstSwap), Distribution<C<T>>(
                deviceList) {

}

template<template<typename > class C, typename T>
template<typename U>
StencilDistribution<C<T>>::StencilDistribution(
        const StencilDistribution<C<U>>& rhs) :
        Distribution<C<T>>(rhs) {
}

template<template<typename > class C, typename T>
StencilDistribution<C<T>>::~StencilDistribution() {
}

template<template<typename > class C, typename T>
bool StencilDistribution<C<T>>::isValid() const {
    return true;
}

template<template<typename > class C, typename T>
void StencilDistribution<C<T>>::startUpload(C<T>& container,
        Event* events) const {
    ASSERT(events != nullptr);
    stencil_distribution_helper::startUpload(container, events, this->_north,
            this->_west, this->_south, this->_east, this->_initialIterationsBeforeFirstSwap, this->_devices);
}

template<template<typename > class C, typename T>
void StencilDistribution<C<T>>::swap(const C<T>& container, unsigned int iterations) const {
    detail::Event events;
    stencil_distribution_helper::swap(container, &events, this->_north,
          this->_west, this->_south, this->_east, iterations, this->_devices);
}


template<template<typename > class C, typename T>
void StencilDistribution<C<T>>::startDownload(C<T>& container,
        Event* events) const {
    ASSERT(events != nullptr);
    stencil_distribution_helper::startDownload(container, events, this->_north,
            this->_west, this->_south, this->_east, this->_initialIterationsBeforeFirstSwap, this->_devices);
}

template<template<typename > class C, typename T>
size_t StencilDistribution<C<T>>::sizeForDevice(const C<T>& container,
        const std::shared_ptr<detail::Device>& devicePtr) const {
    return stencil_distribution_helper::sizeForDevice<T>(devicePtr,
            container.size(), this->_devices, this->_north, this->_west,
            this->_south, this->_east, this->_initialIterationsBeforeFirstSwap);
}

template<template<typename > class C, typename T>
bool StencilDistribution<C<T>>::dataExchangeOnDistributionChange(
        Distribution<C<T>>& newDistribution) {
    auto block = dynamic_cast<StencilDistribution<C<T>>*>(&newDistribution);

    if (block == nullptr) { // distributions differ => data exchange
        return true;
    } else { // new distribution == block distribution
        if (this->_devices == block->_devices) // same set of devices
                {
            return false; // => no data exchange
        } else {
            return true;  // => data exchange
        }
    }
}

template<template<typename > class C, typename T>
const unsigned int& StencilDistribution<C<T>>::getNorth() const {
    return this->_north;
}

template<template<typename > class C, typename T>
const unsigned int& StencilDistribution<C<T>>::getWest() const {
    return this->_west;
}

template<template<typename > class C, typename T>
const unsigned int& StencilDistribution<C<T>>::getSouth() const {
    return this->_south;
}

template<template<typename > class C, typename T>
const unsigned int& StencilDistribution<C<T>>::getEast() const {
    return this->_east;
}

template<template<typename > class C, typename T>
const detail::Padding& StencilDistribution<C<T>>::getPadding() const {
    return this->_padding;
}

template<template<typename > class C, typename T>
const T& StencilDistribution<C<T>>::getNeutralElement() const {
    return this->_neutral_element;
}

template<template<typename > class C, typename T>
bool StencilDistribution<C<T>>::doCompare(const Distribution<C<T>>& rhs) const {
    bool ret = false;
    // can rhs be casted into block distribution ?
    auto const blockRhs = dynamic_cast<const StencilDistribution*>(&rhs);
    if (blockRhs) {
        ret = true;
    }
    return ret;
}

namespace stencil_distribution_helper {

template<typename T>
size_t sizeForDevice(const std::shared_ptr<Device>& devicePtr,
        const typename Vector<T>::size_type size, const DeviceList& devices,
        unsigned int north, unsigned int west, unsigned int south,
        unsigned int east, unsigned int initialIterationsBeforeFirstSwap) {
    LOG_DEBUG_INFO("Vector Version , north: ", north, " and south: ", south,
            " are not considered for the size.");
    auto id = devicePtr->id();
                              if(initialIterationsBeforeFirstSwap){};
    if (id < devices.size() - 1) {
        auto s = size / devices.size();
        s += west + east;
        return s;
    } else { // "last" device
        auto s = size / devices.size();
        s += size % devices.size();
        s += west + east;
        return s;
    }
}

template<typename T>
size_t sizeForDevice(const std::shared_ptr<Device>& devicePtr,
        const typename Matrix<T>::size_type size, const DeviceList& devices,
        unsigned int north, unsigned int west, unsigned int south,
        unsigned int east, unsigned int initialIterationsBeforeFirstSwap) {
    LOG_DEBUG_INFO("Matrix Version, west: ",north, south, west, " and east: ", east,
            " are not considered for the size.");
      auto id = devicePtr->id();
                              if(initialIterationsBeforeFirstSwap){};
      if (id == devices.size() - 1 && devices.size() > 1) {
          auto s = (size.rowCount() / devices.size()) * size.columnCount();
          s += (size.rowCount() % devices.size()) * size.columnCount();
          s += initialIterationsBeforeFirstSwap * north * size.columnCount();
                                LOG_DEBUG("s ", s);
          return s;
      } else if (id == 0 && devices.size() == 1) {
          auto s = (size.rowCount() / devices.size()) * size.columnCount();
                                LOG_DEBUG("t ", s);
          return s;
      } else if (id == 0) {
          auto s = (size.rowCount() / devices.size()) * size.columnCount();
          s += initialIterationsBeforeFirstSwap * south * size.columnCount();
                                LOG_DEBUG("u ", s);
          return s;
      } else {
          auto s = size.rowCount() / devices.size() * size.columnCount();
          s += (initialIterationsBeforeFirstSwap * (north + south)) * size.columnCount();
                                LOG_DEBUG("v ", s);
          return s;
      }


                              /*auto id = devicePtr->id();
    if (id == devices.size() - 1 && devices.size() > 1) {
        auto s = (size.rowCount() / devices.size()) * size.columnCount();
        s += (size.rowCount() % devices.size()) * size.columnCount();
        //s += (north) * size.columnCount();
                              LOG_DEBUG("s ", s);
        return s;
    } else if (id == 0 && devices.size() == 1) {
        auto s = (size.rowCount() / devices.size()) * size.columnCount();
                              LOG_DEBUG("t ", s);
        return s;
    } else if (id == 0) {
        auto s = (size.rowCount() / devices.size()) * size.columnCount();
        //s += south * size.columnCount();
                              LOG_DEBUG("u ", s);
        return s;
    } else {
        auto s = size.rowCount() / devices.size() * size.columnCount();
        //s += (north + south) * size.columnCount();
                              LOG_DEBUG("v ", s);
        return s;
    }*/
}

template<typename T>
void startUpload(Vector<T>& vector, Event* events, unsigned int north,
        unsigned int west, unsigned int south, unsigned int east, unsigned int initialIterationsBeforeFirstSwap,
        detail::DeviceList devices) {
    ASSERT(events != nullptr);
    LOG_DEBUG_INFO("Vector Version , north: ", north, " and south: ", south,
            " are not considered for the upload.");
                              if(initialIterationsBeforeFirstSwap){};
//  size_t offset       = 0;
    size_t deviceOffset = 0;
    size_t devSize = devices.size();
    size_t hostOffset = 0;

    for (size_t i = 0; i < devSize; ++i) {
        auto& devicePtr = devices[i];
        auto& buffer = vector.deviceBuffer(*devicePtr);

        auto size = buffer.size();

        auto event = devicePtr->enqueueWrite(buffer,
                vector.hostBuffer().begin(), size, deviceOffset, hostOffset);
        events->insert(event);

        hostOffset += size - (east + west);
        deviceOffset = 0; // after the first device, the device offset is 0
    }
}
template<typename T>
void startUpload(Matrix<T>& matrix, Event* events, unsigned int north,
        unsigned int west, unsigned int south, unsigned int east, unsigned int initialIterationsBeforeFirstSwap,
        detail::DeviceList devices) {
    ASSERT(events != nullptr);
    LOG_DEBUG_INFO("Matrix Version , west: ", west, " and east: ", east,
            " are not considered for the upload.");
    if(initialIterationsBeforeFirstSwap){};
    size_t northOverlap = north * matrix.size().columnCount();
    size_t southOverlap = south * matrix.size().columnCount();

    size_t northOverlapWithIter = initialIterationsBeforeFirstSwap * northOverlap;
    size_t southOverlapwithIter = initialIterationsBeforeFirstSwap * southOverlap;

    size_t hostOffset   = 0;
    size_t deviceOffset = 0;
    size_t devSize      = devices.size();

    for(size_t i = 0; i < devSize; ++i) {
      auto& devicePtr = devices[i];
      auto& buffer    = matrix.deviceBuffer(*devicePtr);

      auto size = buffer.size();

      auto event = devicePtr->enqueueWrite(buffer,
                                      matrix.hostBuffer().begin(),
                                      size,
                                      deviceOffset,
                                      hostOffset);
      events->insert(event);

      hostOffset += (buffer.size() - (northOverlapWithIter - southOverlapwithIter));
      if(i==0) hostOffset -= northOverlapWithIter;

      deviceOffset = 0; // after the first device, the device offset is 0
     }
}

template<typename T>
void swap(const Vector<T>& vector, Event* events, unsigned int north, unsigned int west,
          unsigned int south, unsigned int east, unsigned int iterations, detail::DeviceList devices) {
    /*
     *
     *
     *
     * TODO TODO TODO
     *
     *
     */
    ASSERT(events != nullptr);
    LOG_DEBUG_INFO("Vector Version , north: ", north, " and south: ", south,
            " are not considered for the upload.");

//  size_t offset       = 0;
    size_t deviceOffset = 0;
    size_t devSize = devices.size();
    size_t hostOffset = 0;

    for (size_t i = 0; i < devSize; ++i) {
        auto& devicePtr = devices[i];
        auto& buffer = vector.deviceBuffer(*devicePtr);

        auto size = buffer.size();

        auto event = devicePtr->enqueueWrite(buffer,
                vector.hostBuffer().begin(), size, deviceOffset, hostOffset);
        events->insert(event);

        hostOffset += size - (east + west);
        deviceOffset = 0; // after the first device, the device offset is 0
    }
}

template<typename T>
void swap(const Matrix<T>& matrix, Event* events, unsigned int north, unsigned int west,
          unsigned int south, unsigned int east, unsigned int iterations, detail::DeviceList devices){
    LOG_DEBUG_INFO("Matrix Version , west: ", west, " and east: ", east,
            " are not considered for the swap.");

    // create temporary vectors
    std::vector<std::vector<T>> vs(2*(devices.size() - 1));

    LOG_DEBUG("SWAP START");
    size_t devSize = devices.size();
    size_t deviceOffset = 0;

    size_t southOverlap = south * matrix.size().columnCount();
    size_t northOverlap = north * matrix.size().columnCount();

    size_t iterationsSouthOverlap = iterations * southOverlap;
    size_t iterationsNorthOverlap = iterations * northOverlap;

    events->wait();

    int k = 0;
    for(size_t i = 0; i < devSize; ++i) {

        auto& devicePtr = devices[i];
        auto id = devicePtr->id();
        auto& buffer = matrix.deviceBuffer(*devicePtr);

        if (id == devSize - 1 && devSize > 1) {
            vs[k].resize(iterationsSouthOverlap);
            deviceOffset = iterationsNorthOverlap;
            auto event = devicePtr->enqueueRead(buffer, vs[k].begin(), vs[k].size(), deviceOffset, 0);
            events->insert(event);
            k++;
        } else if (id == 0 && devSize == 1) {
            LOG_ERROR("It is stupid to call swap when there is only one device");
        } else if (id == 0) {
            vs[k].resize(iterationsNorthOverlap);
            deviceOffset = buffer.size() - iterationsNorthOverlap - iterationsSouthOverlap;
            auto event = devicePtr->enqueueRead(buffer, vs[k].begin(), vs[k].size(), deviceOffset, 0);
            events->insert(event);
            k++;
        } else {
            vs[k].resize(iterationsSouthOverlap);
            deviceOffset = iterationsNorthOverlap;
            auto event = devicePtr->enqueueRead(buffer, vs[k].begin(), vs[k].size(), deviceOffset, 0);
            events->insert(event);
            k++;
            vs[k].resize(iterationsNorthOverlap);
            deviceOffset = buffer.size() - iterationsSouthOverlap - iterationsNorthOverlap;
            event = devicePtr->enqueueRead(buffer, vs[k].begin(), vs[k].size(), deviceOffset, 0);
            events->insert(event);
            k++;
        }
    }
    events->wait();

    for(unsigned int i = 0; i<vs.size(); i++){
        LOG_DEBUG("Vector size ", i, " ", vs[i].size());
        /*typename std::vector<T>::iterator itr = vs[i].begin();
        unsigned int j = 0;
        for (itr = vs[i].begin(); itr != vs[i].end(); ++itr) {
            LOG_DEBUG(j, " ", *itr);
            j++;
        }*/
    }

    for(unsigned int i = 0; i<vs.size()-1; i=i+2) {
        LOG_DEBUG(i);
        std::swap(vs[i], vs[i+1]);
    }

    for(unsigned int i = 0; i<vs.size(); i++){
        LOG_DEBUG("Vector size ", i, " ", vs[i].size());
    }

    size_t j = 0;
    for(size_t i = 0; i < devSize; i++) {
        if(i!=0)
            j = 2*i-1;

        auto& devicePtr = devices[i];
        auto id = devicePtr->id();
        auto& buffer = matrix.deviceBuffer(*devicePtr);

        if (id == devSize - 1 && devSize > 1) {
            deviceOffset = 0;
            auto event = devicePtr->enqueueWrite(buffer,
                                               vs[j].begin(),
                                               vs[j].size(),
                                               deviceOffset, 0);
            events->insert(event);
        } else if (id == 0 && devSize == 1) {
            LOG_ERROR("It is stupid to call swap when there is only one device");
        } else if (id == 0) {
            deviceOffset = buffer.size() - iterationsSouthOverlap;
            auto event = devicePtr->enqueueWrite(buffer,
                                                 vs[i].begin(),
                                                 vs[i].size(),
                                                 deviceOffset, 0);
            events->insert(event);
        } else {
            deviceOffset = 0;
            auto event = devicePtr->enqueueWrite(buffer,
                                                 vs[j].begin(),
                                                 vs[j].size(),
                                                 deviceOffset, 0);
            events->insert(event);
            deviceOffset = buffer.size() - iterationsSouthOverlap;
            event = devicePtr->enqueueWrite(buffer,
                                                 vs[j+1].begin(),
                                                 vs[j+1].size(),
                                                 deviceOffset, 0);
            events->insert(event);
        }
    }
    events->wait();
    LOG_DEBUG("SWAP END");
}

template<typename T>
void startDownload(Vector<T>& vector, Event* events, unsigned int north,
        unsigned int west, unsigned int south, unsigned int east, unsigned int initialIterationsBeforeFirstSwap,
        detail::DeviceList devices) {
    ASSERT(events != nullptr);
    LOG_DEBUG_INFO("Vector Version , north: ", north, " and south: ", south,
            " are not considered for the download.");
    size_t offset = 0;
    if(initialIterationsBeforeFirstSwap){};
    for (auto& devicePtr : devices) {
        auto& buffer = vector.deviceBuffer(*devicePtr);

        int size = buffer.size() - (west + east);

        auto event = devicePtr->enqueueRead(buffer, vector.hostBuffer().begin(),
                size, west, offset);
        offset += size;
        events->insert(event);
    }

// mark data on device as out of date !
// TODO: find out why? -> ask matthias
//  matrix.dataOnHostModified();
}

template<typename T>
void startDownload(Matrix<T>& matrix, Event* events, unsigned int north,
        unsigned int west, unsigned int south, unsigned int east, unsigned int initialIterationsBeforeFirstSwap,
        detail::DeviceList devices) {
    if(initialIterationsBeforeFirstSwap){};
    ASSERT(events != nullptr);
    LOG_DEBUG_INFO("Matrix Version , west: ", west, " and east: ", east,
            " are not considered for the download.");
    size_t offset = 0;

    size_t northOverlap = north * matrix.size().columnCount();
    size_t southOverlap = south * matrix.size().columnCount();

    size_t northOverlapwithIter = initialIterationsBeforeFirstSwap * northOverlap;
    size_t southOverlapwithIter = initialIterationsBeforeFirstSwap * southOverlap;

    size_t deviceOffset = 0;

    for (auto& devicePtr : devices) {
        auto id = devicePtr->id();
        auto& buffer = matrix.deviceBuffer(*devicePtr);

        size_t size = buffer.size();

        if (id == devices.size() - 1 && devices.size() > 1) {
            deviceOffset = northOverlapwithIter;
            size -= northOverlapwithIter;
        } else if (id == 0 && devices.size() == 1) {
        } else if (id == 0) {
            size -= southOverlapwithIter;
        } else {
            deviceOffset = northOverlapwithIter;
            size -= (southOverlapwithIter + northOverlapwithIter);
        }

        auto event = devicePtr->enqueueRead(buffer, matrix.hostBuffer().begin(),
                size, deviceOffset, offset);
        offset += size;
        events->insert(event);
    }

// mark data on device as out of date !
// TODO: find out why? -> ask matthias
//  matrix.dataOnHostModified();
}

} // namespace stencil_distribution_helper

} // namespace detail

} // namespace skelcl

#endif // STENCILDISTRIBUTION_DEF _H_
