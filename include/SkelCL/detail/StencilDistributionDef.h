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
		auto s = size.elemCount() / devices.size();
        //s += (size.rowCount() % devices.size()) * size.columnCount();
        //s += (north) * size.columnCount();
                              LOG_DEBUG("s ", s);
		return s;
	} else if (id == 0 && devices.size() == 1) {
		auto s = size.elemCount() / devices.size();
                              LOG_DEBUG("t ", s);
		return s;
	} else if (id == 0) {
		auto s = size.elemCount() / devices.size();
        //s += (size.rowCount() % devices.size()) * size.columnCount();
        //s += south * size.columnCount();
                              LOG_DEBUG("u ", s);
		return s;
    } else {
		auto s = size.elemCount() / devices.size();
        //s += (north + south) * size.columnCount();
                              LOG_DEBUG("v ", s);
		return s;
	}
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
	size_t hostOffset = 0; //????

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
// some shortcuts ...
//	auto columnCount = matrix.size().columnCount();

// create vectors for additional rows
// initialize with neutral value for SCL_NEUTRAL
// (override this in differnt case later)
//std::vector<T> paddingTop(north * columnCount, neutralElement);
//std::vector<T> paddingBottom(south * columnCount, neutralElement);

//Matrix<T>::host_buffer_type paddingTop(newSize, neutralElement);
//Matrix<T>::host_buffer_type paddingBottom(newSize, neutralElement);

	/*if (padding == detail::Padding::NEAREST) {
	 paddingTop.clear();
	 paddingBottom.clear();

	 for(unsigned int row = 0; row < north; ++row){
	 for(unsigned int col = 0; col<columnCount; ++col){
	 T valFront = matrix(row, col);
	 paddingTop.push_back(valFront);
	 }
	 }

	 for (unsigned int row = 0; row < south; ++row) {
	 for (unsigned int col = 0; col < columnCount; ++col) {
	 T valBack = matrix(matrix.size().rowCount()-south+row, col);
	 paddingBottom.push_back(valBack);
	 }
	 }
	 }*/
//auto& firstDevicePtr = devices.front();
// Upload top padding to first device
    /*if(padding == detail::Padding::NEAREST && paddingTop.size() > 0){
     LOG_DEBUG("Padding top with size: ", paddingTop.size());
     auto eventTop = firstDevicePtr->enqueueWrite(
     matrix.deviceBuffer(*firstDevicePtr), paddingTop.begin(),
     paddingTop.size(), 0);
     events->insert(eventTop);
     }*/

	size_t hostOffset = 0;
    //size_t deviceOffset = 0;
	size_t devSize = devices.size();
    /*std::vector<T> pad(north * matrix.size().columnCount());
    for(unsigned int row = 0; row < north; ++row){
        for(unsigned int col = 0; col<columnCount; ++col){
            T valFront = matrix(row, col);
            pad.push_back(valFront);
        }
    }*/
    if(south&&north) {};

    for (size_t i = 0; i < devSize; i++) {
        auto& devicePtr = devices[i];
		auto& buffer = matrix.deviceBuffer(*devicePtr);

        //auto eventData = devicePtr->enqueueWrite(buffer,
        //       pad.begin(), 0);
        //events->insert(eventData);
        //deviceOffset = pad.size();

        auto size = buffer.size();

        auto eventData = devicePtr->enqueueWrite(buffer,
                matrix.hostBuffer().begin(), hostOffset);
        events->insert(eventData);

        hostOffset += size;

		// offset += (buffer.size()-2*_overlap_radius
		//            *_size.column_count-deviceoffset);
        //deviceOffset = 0; // after the first device, the device offset is 0
	}

	/*if(padding == detail::Padding::NEAREST && paddingBottom.size() > 0){
	 LOG_DEBUG("Padding bottom with size: ", paddingBottom.size());
	 // Upload bottom padding at the end of last device
	 auto& lastDevicePtr = devices.back();
	 // calculate offset on the device ...
	 deviceOffset = matrix.deviceBuffer(*lastDevicePtr).size()
	 - paddingBottom.size();

	 LOG_DEBUG("Device Offset: ", deviceOffset);
	 LOG_DEBUG(matrix.deviceBuffer(*lastDevicePtr).size());
	 auto eventBottom = firstDevicePtr->enqueueWrite(matrix.deviceBuffer(*lastDevicePtr),
	 paddingBottom.begin(), paddingBottom.size(), deviceOffset, 0);
	 events->insert(eventBottom);
	 }*/
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
    LOG_DEBUG("A");
    if(north&&south){}
    for (auto& devicePtr : devices) {
        LOG_DEBUG("sA");
		auto& buffer = matrix.deviceBuffer(*devicePtr);
        //size_t deviceOffset = 0;
        size_t size = buffer.size();

        /*if(devicePtr->id()==0 && devices.size() > 1){
            size -= south * matrix.size().columnCount();
        } else if (devicePtr->id() == 0) {

        } else if (devicePtr->id() == devices.size() - 1 && devices.size() > 1) {
            size -= north * matrix.size().columnCount();
            //deviceOffset = north * matrix.size().columnCount();
        } else {
            size -= (north + south) * matrix.size().columnCount();
            deviceOffset = north * matrix.size().columnCount();
        }*/
LOG_DEBUG("size", size);
auto event = devicePtr->enqueueRead(buffer, matrix.hostBuffer().begin(),
                offset);
		offset += size;
        events->insert(event);
        LOG_DEBUG("Ad");
	}

// mark data on device as out of date !
// TODO: find out why? -> ask matthias
//  matrix.dataOnHostModified();
}

} // namespace stencil_distribution_helper

} // namespace detail

} // namespace skelcl

#endif // STENCILDISTRIBUTION_DEF _H_
