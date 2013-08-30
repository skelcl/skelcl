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
		const T neutral_element, const DeviceList& deviceList) :
		_north(north), _west(west), _south(south), _east(east), _padding(
				padding), _neutral_element(neutral_element), Distribution<C<T>>(
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
			this->_west, this->_south, this->_east, this->_devices);
}

template<template<typename > class C, typename T>
void StencilDistribution<C<T>>::swap(C<T>& container) const {
    detail::Event events;
    stencil_distribution_helper::swap(container, &events, this->_north,
          this->_west, this->_south, this->_east, this->_devices);
}


template<template<typename > class C, typename T>
void StencilDistribution<C<T>>::startDownload(C<T>& container,
		Event* events) const {
	ASSERT(events != nullptr);
	stencil_distribution_helper::startDownload(container, events, this->_north,
			this->_west, this->_south, this->_east, this->_devices);
}

template<template<typename > class C, typename T>
size_t StencilDistribution<C<T>>::sizeForDevice(const C<T>& container,
		const std::shared_ptr<detail::Device>& devicePtr) const {
	return stencil_distribution_helper::sizeForDevice<T>(devicePtr,
			container.size(), this->_devices, this->_north, this->_west,
			this->_south, this->_east);
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
		unsigned int east) {
	LOG_DEBUG_INFO("Vector Version , north: ", north, " and south: ", south,
			" are not considered for the size.");
	auto id = devicePtr->id();
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
		unsigned int east) {
    LOG_DEBUG_INFO("Matrix Version, west: ",north, south, west, " and east: ", east,
			" are not considered for the size.");
      auto id = devicePtr->id();
      if (id == devices.size() - 1 && devices.size() > 1) {
          auto s = (size.rowCount() / devices.size()) * size.columnCount();
          s += (size.rowCount() % devices.size()) * size.columnCount();
          s += (north) * size.columnCount();
                                LOG_DEBUG("s ", s);
          return s;
      } else if (id == 0 && devices.size() == 1) {
          auto s = (size.rowCount() / devices.size()) * size.columnCount();
                                LOG_DEBUG("t ", s);
          return s;
      } else if (id == 0) {
          auto s = (size.rowCount() / devices.size()) * size.columnCount();
          s += south * size.columnCount();
                                LOG_DEBUG("u ", s);
          return s;
      } else {
          auto s = size.rowCount() / devices.size() * size.columnCount();
          s += (north + south) * size.columnCount();
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
		unsigned int west, unsigned int south, unsigned int east,
		detail::DeviceList devices) {
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
void startUpload(Matrix<T>& matrix, Event* events, unsigned int north,
		unsigned int west, unsigned int south, unsigned int east,
		detail::DeviceList devices) {
	ASSERT(events != nullptr);
	LOG_DEBUG_INFO("Matrix Version , west: ", west, " and east: ", east,
            " are not considered for the upload.");

    size_t northOverlap = north * matrix.size().columnCount();
    size_t southOverlap = south * matrix.size().columnCount();

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

      hostOffset += (buffer.size() - northOverlap - southOverlap);

      deviceOffset = 0; // after the first device, the device offset is 0
     }


    /*size_t hostOffset = 0;
	size_t devSize = devices.size();
    if(south&&north) {};
    for (size_t i = 0; i < devSize; i++) {
        auto& devicePtr = devices[i];
		auto& buffer = matrix.deviceBuffer(*devicePtr);

        auto size = buffer.size();

        auto eventData = devicePtr->enqueueWrite(buffer,
                matrix.hostBuffer().begin(), hostOffset);
        events->insert(eventData);
        hostOffset += size;
    }*/
}

template<typename T>
void swap(Vector<T>& vector, Event* events, unsigned int north, unsigned int west,
          unsigned int south, unsigned int east, detail::DeviceList devices) {
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
void swap(Matrix<T>& matrix, Event* events, unsigned int north, unsigned int west,
          unsigned int south, unsigned int east, detail::DeviceList devices){
    LOG_DEBUG_INFO("Matrix Version , west: ", west, " and east: ", east,
            " are not considered for the swap.");
    std::vector<std::vector<T> > downloadedVectors;
    LOG_DEBUG("SWAP START");
    size_t devSize = devices.size();
    size_t deviceOffset = 0;

    size_t southOverlap = south * matrix.size().columnCount();
    size_t northOverlap = north * matrix.size().columnCount();
    int k = 0;
    for(size_t i = 0; i < devSize; ++i) {

        auto& devicePtr = devices[i];
        auto id = devicePtr->id();
        auto& buffer = matrix.deviceBuffer(*devicePtr);

        if (id == devSize - 1 && devSize > 1) {
            downloadedVectors.push_back(std::vector<T>());
            std::vector<T> vec(southOverlap, T());
            deviceOffset = northOverlap;
            auto event = devicePtr->enqueueRead(buffer, vec.begin(), southOverlap, deviceOffset, 0);
            events->insert(event);
            downloadedVectors[k] = vec;
            k++;
            //downloadedVectors.push_back(vec);
        } else if (id == 0 && devSize == 1) {
            LOG_ERROR("It is stupid to call swap when there is only one device");
        } else if (id == 0) {
            downloadedVectors.push_back(std::vector<T>());
            std::vector<T> vec(northOverlap, T());
            deviceOffset = buffer.size() - southOverlap - northOverlap;
            auto event = devicePtr->enqueueRead(buffer, vec.begin(), northOverlap, deviceOffset, 0);
            events->insert(event);
            downloadedVectors[k] = vec;
            k++;
            //downloadedVectors.push_back(vec);
        } else {
            /*std::vector<T> vecNorth(northOverlap, T());
            std::vector<T> vecSouth(southOverlap, T());
            deviceOffset = 0;
            auto event = devicePtr->enqueueRead(buffer, vecNorth.begin(), vecNorth.size(), deviceOffset, 0);
            events->insert(event);
            deviceOffset = buffer.size() - northOverlap;
            event = devicePtr->enqueueRead(buffer, vecSouth.begin(), vecSouth.size(), deviceOffset, 0);
            events->insert(event);
            downloadedVectors.push_back(vecNorth);
            downloadedVectors.push_back(vecSouth);*/
        }
    }

    for(unsigned int i = 0; i<downloadedVectors.size(); i++){
        LOG_DEBUG("Vector size ", i, " ", downloadedVectors.at(i).size());
    }

    for(unsigned int i = 0; i<downloadedVectors.size()-1; i=i+2) {
        LOG_DEBUG(i);
        std::swap(downloadedVectors.at(i), downloadedVectors.at(i+1));
    }

    for(unsigned int i = 0; i<downloadedVectors.size(); i++){
        LOG_DEBUG("Vector size ", i, " ", downloadedVectors.at(i).size());
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
                                               downloadedVectors.at(j).begin(),
                                               downloadedVectors.at(j).size(),
                                               deviceOffset, 0);
            events->insert(event);
        } else if (id == 0 && devSize == 1) {
            LOG_ERROR("It is stupid to call swap when there is only one device");
        } else if (id == 0) {
            deviceOffset = buffer.size() - southOverlap;
            auto event = devicePtr->enqueueWrite(buffer,
                                                 downloadedVectors.at(i).begin(),
                                                 southOverlap,
                                                 deviceOffset, 0);
            events->insert(event);
        } else {
            /*deviceOffset = 0;
            auto event = devicePtr->enqueueWrite(buffer,
                                                 downloadedVectors.at(j).begin(),
                                                 downloadedVectors.at(j).size(),
                                                 deviceOffset, 0);
            events->insert(event);
            deviceOffset = buffer.size() - southOverlap;
            event = devicePtr->enqueueWrite(buffer,
                                                 downloadedVectors.at(j+1).begin(),
                                                 downloadedVectors.at(j+1).size(),
                                                 deviceOffset, 0);
            events->insert(event);*/
        }
    }
    LOG_DEBUG("SWAP END");
}

template<typename T>
void startDownload(Vector<T>& vector, Event* events, unsigned int north,
		unsigned int west, unsigned int south, unsigned int east,
		detail::DeviceList devices) {
	ASSERT(events != nullptr);
	LOG_DEBUG_INFO("Vector Version , north: ", north, " and south: ", south,
			" are not considered for the download.");
	size_t offset = 0;

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
		unsigned int west, unsigned int south, unsigned int east,
		detail::DeviceList devices) {
    ASSERT(events != nullptr);
	LOG_DEBUG_INFO("Matrix Version , west: ", west, " and east: ", east,
			" are not considered for the download.");
    size_t offset = 0;

    size_t northOverlap = north * matrix.size().columnCount();
    size_t southOverlap = south * matrix.size().columnCount();

    size_t deviceOffset = 0;

    for (auto& devicePtr : devices) {
        auto id = devicePtr->id();
        auto& buffer = matrix.deviceBuffer(*devicePtr);

        size_t size = buffer.size();

        if (id == devices.size() - 1 && devices.size() > 1) {
            deviceOffset = northOverlap;
            size -= northOverlap;
        } else if (id == 0 && devices.size() == 1) {
        } else if (id == 0) {
            size -= southOverlap;
        } else {
            deviceOffset = northOverlap;
            size -= (southOverlap + northOverlap);
        }

        auto event = devicePtr->enqueueRead(buffer, matrix.hostBuffer().begin(),
                size, deviceOffset, offset);
        offset += size;
        events->insert(event);
    }

    if(north&&south){}
    /*for (auto& devicePtr : devices) {
		auto& buffer = matrix.deviceBuffer(*devicePtr);
        size_t size = buffer.size();

        auto event = devicePtr->enqueueRead(buffer, matrix.hostBuffer().begin(),
                offset);
        events->insert(event);
        offset += size;
    }*/

// mark data on device as out of date !
// TODO: find out why? -> ask matthias
//  matrix.dataOnHostModified();
}

} // namespace stencil_distribution_helper

} // namespace detail

} // namespace skelcl

#endif // STENCILDISTRIBUTION_DEF _H_
