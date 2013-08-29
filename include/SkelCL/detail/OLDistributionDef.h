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
/// \file OLDistributionDef.h
///
/// Working for Matrix-Version.
///
///	\author Stefan Breuer <s_breu03@uni-muenster.de>
///
#ifndef OL_DISTRIBUTION_DEF_H_
#define OL_DISTRIBUTION_DEF_H_

namespace skelcl {

namespace detail {

template<template<typename > class C, typename T>
OLDistribution<C<T>>::OLDistribution(const unsigned int overlapRadius,
        const detail::Padding padding, const T neutral_element,
		const DeviceList& deviceList) :
        _overlap_radius(overlapRadius), _padding(padding), _neutral_element(neutral_element), Distribution<C<T>>(deviceList) {
}

template<template<typename > class C, typename T>
template<typename U>
OLDistribution<C<T>>::OLDistribution(const OLDistribution<C<U>>& rhs) :
		Distribution<C<T>>(rhs) {
}

template<template<typename > class C, typename T>
OLDistribution<C<T>>::~OLDistribution() {
}

template<template<typename > class C, typename T>
bool OLDistribution<C<T>>::isValid() const {
	return true;
}

template<template<typename > class C, typename T>
void OLDistribution<C<T>>::startUpload(C<T>& container, Event* events) const {
	ASSERT(events != nullptr);
	ol_distribution_helper::startUpload(container, events,
			this->_overlap_radius, this->_padding, this->_neutral_element,
			this->_devices);
}

template<template<typename > class C, typename T>
void OLDistribution<C<T>>::startDownload(C<T>& container, Event* events) const {
	ASSERT(events != nullptr);
	ol_distribution_helper::startDownload(container, events,
			this->_overlap_radius, this->_devices);
}

template<template<typename > class C, typename T>
size_t OLDistribution<C<T>>::sizeForDevice(const C<T>& container,
		const std::shared_ptr<detail::Device>& devicePtr) const {
	return ol_distribution_helper::sizeForDevice<T>(devicePtr, container.size(),
			this->_devices, this->_overlap_radius);
}

template<template<typename > class C, typename T>
bool OLDistribution<C<T>>::dataExchangeOnDistributionChange(
		Distribution<C<T>>& newDistribution) {
	auto block = dynamic_cast<OLDistribution<C<T>>*>(&newDistribution);

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
const unsigned int& OLDistribution<C<T>>::getOverlapRadius() const {
	return this->_overlap_radius;
}

template<template<typename > class C, typename T>
const detail::Padding& OLDistribution<C<T>>::getPadding() const {
	return this->_padding;
}

template<template<typename > class C, typename T>
const T& OLDistribution<C<T>>::getNeutralElement() const {
	return this->_neutral_element;
}

template<template<typename > class C, typename T>
bool OLDistribution<C<T>>::doCompare(const Distribution<C<T>>& rhs) const {
	bool ret = false;
	// can rhs be casted into block distribution ?
	auto const blockRhs = dynamic_cast<const OLDistribution*>(&rhs);
	if (blockRhs) {
		ret = true;
	}
	return ret;
}

namespace ol_distribution_helper {

template<typename T>
size_t sizeForDevice(const std::shared_ptr<Device>& devicePtr,
		const typename Vector<T>::size_type size, const DeviceList& devices,
		unsigned int overlapRadius) {
	LOG_DEBUG("Vector Version");
	auto id = devicePtr->id();
	if (id < devices.size() - 1) {
		auto s = size / devices.size();
		s += overlapRadius + overlapRadius;
		return s;
	} else { // "last" device
		auto s = size / devices.size();
		s += size % devices.size();
		s += overlapRadius + overlapRadius;
		return s;
	}
}

template<typename T>
size_t sizeForDevice(const std::shared_ptr<Device>& devicePtr,
		const typename Matrix<T>::size_type size, const DeviceList& devices,
		const unsigned int overlapRadius) {
    auto id = devicePtr->id();
    if(id == devices.size() - 1){
         auto s = size.elemCount() / devices.size();
         s += (size.rowCount() % devices.size()) * size.columnCount();
         s += 2 * overlapRadius * size.columnCount();
         return s;
    } else {
         auto s = size.elemCount() / devices.size();
         s += 2 * overlapRadius * size.columnCount();
         return s;
    }
}

template<typename T>
void startUpload(Vector<T>& vector, Event* events, unsigned int overlapRadius,
		detail::Padding padding, T neutralElement, detail::DeviceList devices) {
	ASSERT(events != nullptr);
	std::vector<T> paddingFront;
	std::vector<T> paddingBack;

	switch (padding) {
	case Padding::NEAREST:
		paddingFront.resize(overlapRadius, vector.front());
		paddingBack.resize(overlapRadius, vector.back());
		break;
	case Padding::NEUTRAL:
        paddingFront.resize(overlapRadius, neutralElement);
        paddingBack.resize(overlapRadius, neutralElement);
        break;
    case Padding::NEAREST_INITIAL:
        paddingFront.resize(overlapRadius, neutralElement);
        paddingBack.resize(overlapRadius, neutralElement);
        break;
	}

	// Upload front paddint to first device
	auto& firstDevicePtr = devices.front();
	auto event = firstDevicePtr->enqueueWrite(
			vector.deviceBuffer(*firstDevicePtr), paddingFront.begin(),
            paddingFront.size());
	events->insert(event);

//  size_t offset       = 0;
	size_t deviceOffset = paddingFront.size();
	size_t devSize = devices.size();
	size_t hostOffset = 0; //????

	for (size_t i = 0; i < devSize; ++i) {
		auto& devicePtr = devices[i];
		auto& buffer = vector.deviceBuffer(*devicePtr);

		auto size = buffer.size();

		if (i == 0)
			size -= paddingFront.size();
		if (i == devSize - 1)
			size -= paddingBack.size();

		event = devicePtr->enqueueWrite(buffer, vector.hostBuffer().begin(),
				size, deviceOffset, hostOffset);
		events->insert(event);

		hostOffset += size - overlapRadius;
		deviceOffset = 0; // after the first device, the device offset is 0
	}

	// upload back padding at the end of last device
	auto& lastDevicePtr = devices.back();
	// calculate offset on the device ...
	deviceOffset = vector.deviceBuffer(*lastDevicePtr).size()
			- paddingBack.size();

	event = lastDevicePtr->enqueueWrite(vector.deviceBuffer(*lastDevicePtr),
			paddingBack.begin(), paddingBack.size(), deviceOffset);
	events->insert(event);
}

template<typename T>
void startUpload(Matrix<T>& matrix, Event* events, unsigned int overlapRadius,
		detail::Padding padding, T neutralElement, detail::DeviceList devices) {
	ASSERT(events != nullptr);

	// some shortcuts ...
	auto columnCount = matrix.size().columnCount();

	// create vectors for additional rows
	// initialize with neutral value for SCL_NEUTRAL
	// (override this in differnt case later)
	auto newSize = overlapRadius * columnCount;
    //std::vector<T> paddingTop(newSize, neutralElement);
    //std::vector<T> paddingBottom(newSize, neutralElement);

    typename Matrix<T>::host_buffer_type paddingTop(newSize, neutralElement);
    typename Matrix<T>::host_buffer_type paddingBottom(newSize, neutralElement);

    if (padding == detail::Padding::NEAREST) {
		paddingTop.clear();
		paddingBottom.clear();

        for (unsigned int row = 0; row < overlapRadius; row++) {
            for (unsigned int col = 0; col < columnCount; col++) {
                T valFront = matrix(row, col);
                T valBack = matrix(matrix.size().rowCount()-overlapRadius+row, col);
                paddingTop.push_back(valFront);
                paddingBottom.push_back(valBack);
            }
		}
    }

	// Upload top padding to first device
	auto& firstDevicePtr = devices.front();
	auto event = firstDevicePtr->enqueueWrite(
                matrix.deviceBuffer(*firstDevicePtr), paddingTop.begin(), paddingTop.size(), 0, 0);
	events->insert(event);
    LOG_DEBUG("Uploaded first padding");

	size_t hostOffset = 0;
	size_t deviceOffset = paddingTop.size();
	size_t devSize = devices.size();

	for (size_t i = 0; i < devSize; ++i) {
		auto& devicePtr = devices[i];
		auto& buffer = matrix.deviceBuffer(*devicePtr);

		auto size = buffer.size();

		if (i == 0)
			size -= paddingTop.size();
		if (i == devSize - 1)
			size -= paddingBottom.size();

		devicePtr->enqueueWrite(buffer, matrix.hostBuffer().begin(), size,
                deviceOffset, hostOffset);
		events->insert(event);
        LOG_DEBUG("Uploaded data");

        hostOffset += size;

        if(i == devSize - 1){
            deviceOffset = buffer.size() - paddingBottom.size();

            event = devicePtr->enqueueWrite(
                       buffer,
                       paddingBottom.begin(),
                       paddingBottom.size(),
                       deviceOffset,0 );
            events->insert(event);
        }

		// offset += (buffer.size()-2*_overlap_radius
		//            *_size.column_count-deviceoffset);
		deviceOffset = 0; // after the first device, the device offset is 0
	}

	// Upload bottom padding at the end of last device
    /*auto& lastDevicePtr = devices.back();
cl_context_info datasize = 0;
lastDevicePtr->clContext().getInfo(CL_CONTEXT_NUM_DEVICES, &datasize);
    LOG_DEBUG(datasize);

    // calculate offset on the device ...
    deviceOffset = matrix.deviceBuffer(*lastDevicePtr).size()
            - paddingBottom.size();

    event = firstDevicePtr->enqueueWrite(matrix.deviceBuffer(*lastDevicePtr),
            paddingBottom.begin(), paddingBottom.size(), deviceOffset, 0);
    LOG_DEBUG("Uploaded last padding");
    events->insert(event);*/
}

template<typename T>
void startDownload(Vector<T>& vector, Event* events, unsigned int overlapRadius,
		detail::DeviceList devices) {
	ASSERT(events != nullptr);

	size_t offset = 0;

	for (auto& devicePtr : devices) {
		auto& buffer = vector.deviceBuffer(*devicePtr);

		int size = buffer.size() - (2 * overlapRadius);

		auto event = devicePtr->enqueueRead(buffer, vector.hostBuffer().begin(),
				size, overlapRadius, offset);
		offset += size;
		events->insert(event);
	}

	// mark data on device as out of date !
	// TODO: find out why? -> ask matthias
  vector.dataOnHostModified();
}

template<typename T>
void startDownload(Matrix<T>& matrix, Event* events, unsigned int overlapRadius,
		detail::DeviceList devices) {
	ASSERT(events != nullptr);
	size_t offset = 0;
	for (auto& devicePtr : devices) {
		auto& buffer = matrix.deviceBuffer(*devicePtr);

		size_t overlapSize = overlapRadius * matrix.size().columnCount();

		size_t size = buffer.size() - (2 * overlapSize);

		auto event = devicePtr->enqueueRead(buffer, matrix.hostBuffer().begin(),
                size, overlapSize, offset);
		offset += size;
		events->insert(event);
	}

	// mark data on device as out of date !
	// TODO: find out why? -> ask matthias
  matrix.dataOnHostModified();
}

} // namespace ol_distribution_helper

} // namespace detail

} // namespace skelcl

#endif // OL_DISTRIBUTION_DEF _H_
