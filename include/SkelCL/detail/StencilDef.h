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
/// \file StencilDef.h
///
///	\author Stefan Breuer<s_breu03@uni-muenster.de>
///
#ifndef STENCILDEF_H_
#define STENCILDEF_H_

#include <algorithm>
#include <istream>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.h>
#undef __CL_ENABLE_EXCEPTIONS

#include <ssedit/TempSourceFile.h>
#include <ssedit/Function.h>

#include <pvsutil/Assert.h>
#include <pvsutil/Logger.h>

#include "../Distributions.h"
#include "../Matrix.h"
#include "../Out.h"

#include "../StencilInfo.h"

#include "Device.h"
#include "KernelUtil.h"
#include "Program.h"
#include "Skeleton.h"
#include "Util.h"

namespace skelcl {

//Ausgangskonstruktor
/*template<typename Tin, typename Tout>
Stencil<Tout(Tin)>::Stencil(const Source& source, unsigned int overlap_range,
		detail::Padding padding, Tin neutral_element, const std::string& func) :
		detail::Skeleton(), _userSource(source), _overlap_range(overlap_range), _padding(
				padding), _neutral_element(neutral_element), _funcName(func), _program(
				createAndBuildProgram()) {
	LOG_DEBUG("Create new Stencil object (", this, ")");
}*/

/*template<typename Tin, typename Tout>
Stencil<Tout(Tin)>::Stencil(const Source& source, unsigned int iterations, unsigned int overlap_range,
        detail::Padding padding, Tin neutral_element, const std::string& func) :
    detail::Skeleton(), _userSource(source), _iterations(iterations), _overlap_range(overlap_range), _padding(padding),
        _neutral_element(neutral_element), _funcName(func), _program(createAndBuildProgram()) {
    LOG_DEBUG("Create new Stencil object (", this, ")");
}*/

template<typename Tin, typename Tout>
Stencil<Tout(Tin)>::Stencil(const Source& source, unsigned int north, unsigned int west, unsigned int south, unsigned int east,
                            detail::Padding padding, Tin neutral_element, const std::string& func):
    detail::Skeleton() {
    LOG_DEBUG("Create new Stencil object (", this, ")");
    add(source, north, west, south, east, padding, neutral_element, func);

}

template<typename Tin, typename Tout>
void Stencil<Tout(Tin)>::add(const Source& source, unsigned int north, unsigned int west, unsigned int south, unsigned int east,
         detail::Padding padding, Tin neutral_element, const std::string& func){
    StencilInfo<Tout(Tin)> info(source, north, west, south, east, padding, neutral_element, func);
    _stencilInfos.push_back(info);
    LOG_DEBUG("Added Stencil Shape");
}

// Ausführungsoperator
template<typename Tin, typename Tout>
template<typename ... Args>
Matrix<Tout> Stencil<Tout(Tin)>::operator()(unsigned int iterations, const Matrix<Tin>& in,
        Args&&... args) {
    Matrix<Tout> output;
    this->operator()(iterations, out(output), in, std::forward<Args>(args)...);
    return output;
}

// Ausführungsoperator mit Referenz
template<typename Tin, typename Tout>
template<typename ... Args>
Matrix<Tout>& Stencil<Tout(Tin)>::operator()(unsigned int iterations, Out<Matrix<Tout>> output,
        const Matrix<Tin>& in, Args&&... args) {
    ASSERT(in.rowCount() > 0);
    ASSERT(in.columnCount() > 0);
    _iterations = iterations;
    prepareInput(in);

    prepareAdditionalInput(std::forward<Args>(args)...);

    prepareOutput(output.container(), in);

    execute(output.container(), in, std::forward<Args>(args)...);
    updateModifiedStatus(output, std::forward<Args>(args)...);

    return output.container();
}

// Ausführen
template<typename Tin, typename Tout>
template<typename... Args>
void Stencil<Tout(Tin)>::execute(Matrix<Tout>& output, const Matrix<Tin>& in,
        Args&&... args) {
    ASSERT(in.distribution().isValid());
    ASSERT(output.rowCount() == in.rowCount() && output.columnCount() == in.columnCount());

    auto& stencilInfo = _stencilInfos.front();

    for (auto& devicePtr : in.distribution().devices()) {
        auto& outputBuffer = output.deviceBuffer(*devicePtr);

        LOG_DEBUG("Output Buffer Size: ", outputBuffer.size());

        auto& inputBuffer = in.deviceBuffer(*devicePtr);

        LOG_DEBUG("Input Buffer Size: ", inputBuffer.size());

        cl_uint elements = static_cast<cl_uint>(output.size().elemCount());
        cl_uint local[2] = {(cl_uint) sqrt(devicePtr->maxWorkGroupSize()), (cl_uint) local[0] }; // C, R
        cl_uint global[2] = {
                static_cast<cl_uint>(detail::util::ceilToMultipleOf(output.columnCount(),
                        local[0])),
                static_cast<cl_uint>(detail::util::ceilToMultipleOf(elements / output.rowCount(),
                        local[1]))}; // SUBTILES

        LOG_DEBUG("elements: ", elements, " north: ", stencilInfo.getNorth(), ", south: ", stencilInfo.getSouth());
        LOG_DEBUG("local: ", local[0], ",", local[1], " global: ", global[0],
                ",", global[1]);
        unsigned int i = 0;
        try {
            cl::Kernel kernel(stencilInfo.getProgram().kernel(*devicePtr, "SCL_STENCIL"));

            for(i = 0; i<_iterations; i++) {
                LOG_DEBUG("Iteration ", i);
                if(i%2==0){
                    kernel.setArg(0, inputBuffer.clBuffer());
                    kernel.setArg(1, outputBuffer.clBuffer());
                    kernel.setArg(2, elements);   // elements
                    kernel.setArg(3, stencilInfo.getNorth()); // north
                    kernel.setArg(4, stencilInfo.getWest()); // west
                    kernel.setArg(5, stencilInfo.getSouth()); // south
                    kernel.setArg(6, stencilInfo.getEast()); // east
                    kernel.setArg(7, static_cast<cl_uint>(output.columnCount())); // number of columns
                    //kernel.setArg(5, _iterations);

                    detail::kernelUtil::setKernelArgs(kernel, *devicePtr, 8,
                            std::forward<Args>(args)...);

                    // keep buffers and arguments alive / mark them as in use
                    auto keepAlive = detail::kernelUtil::keepAlive(*devicePtr,
                            inputBuffer.clBuffer(), outputBuffer.clBuffer(), std::forward<Args>(args)...);

                    // after finishing the kernel invoke this function ...
                    auto invokeAfter = [=] () {(void)keepAlive;};
                    devicePtr->enqueue(kernel, cl::NDRange(global[0], global[1]),
                        cl::NDRange(local[0], local[1]), cl::NullRange, // offset
                        invokeAfter);
                } else {
                    kernel.setArg(1, inputBuffer.clBuffer());
                    kernel.setArg(0, outputBuffer.clBuffer());
                    kernel.setArg(2, elements);   // elements
                    kernel.setArg(3, stencilInfo.getNorth()); // north
                    kernel.setArg(4, stencilInfo.getWest()); // west
                    kernel.setArg(5, stencilInfo.getSouth()); // south
                    kernel.setArg(6, stencilInfo.getEast()); // east
                    kernel.setArg(7, static_cast<cl_uint>(output.columnCount())); // number of columns
                    //kernel.setArg(5, _iterations);

                    detail::kernelUtil::setKernelArgs(kernel, *devicePtr, 8,
                    		std::forward<Args>(args)...);

                    // keep buffers and arguments alive / mark them as in use
                    auto keepAlive = detail::kernelUtil::keepAlive(*devicePtr,
                            inputBuffer.clBuffer(), outputBuffer.clBuffer(), std::forward<Args>(args)...);

                    // after finishing the kernel invoke this function ...
                    auto invokeAfter = [=] () {(void)keepAlive;};
                    devicePtr->enqueue(kernel, cl::NDRange(global[0], global[1]),
                            cl::NDRange(local[0], local[1]), cl::NullRange, // offset
                            invokeAfter);
                }
            }
        } catch (cl::Error& err) {
            ABORT_WITH_ERROR(err);
        }
    }
    LOG_INFO("Stencil kernel started");
}

// Eingabe vorbereiten
//TODO: Hier wird jetzt erst einmal davon ausgegangen, dass nur mit einer StencilShape pro Skelett gearbeitet wird.
//Sollte das nicht mehr der Fall sein, muss überlegt werden, wie die Distributions gesetzt werden (natürlich eine Distribution für die Eingabematrix, aber
//in der Distribution prüfen, wie vorgegangen werden soll (größte Stencilshape nur berücksichtigen oder so)...
template<typename Tin, typename Tout>
void Stencil<Tout(Tin)>::prepareInput(const Matrix<Tin>& in) {
    // set distribution
	auto& stencilInfo = _stencilInfos.front();
    in.setDistribution(detail::StencilDistribution<Matrix<Tin>>(stencilInfo.getNorth(), stencilInfo.getWest(), stencilInfo.getSouth(), stencilInfo.getEast(),
    		stencilInfo.getPadding(), stencilInfo.getNeutralElement()));

    // create buffers if required
    in.createDeviceBuffers();

    // copy data to devices
    in.startUpload();
}

// Ausgabe vorbereiten
template<typename Tin, typename Tout>
void Stencil<Tout(Tin)>::prepareOutput(Matrix<Tout>& output,
        const Matrix<Tin>& in) {
    // set size
    if (output.rowCount() != in.rowCount())
        output.resize(typename Matrix<Tout>::size_type(in.rowCount(), in.columnCount()));

    // adopt distribution from in input
    output.setDistribution(in.distribution()); // richtiger typ (Tout)?

    //create buffers if required
    output.createDeviceBuffers();

    output.startUpload();
}

}

#endif /* STENCILDEF_H_ */
