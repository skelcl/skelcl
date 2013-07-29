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

//Konstruktor für Matrix
template<typename Tin, typename Tout>
Stencil<Tout(Tin)>::Stencil(const Source& source, unsigned int north, unsigned int west, unsigned int south, unsigned int east,
                            detail::Padding padding, Tin neutral_element, const std::string& func):
    detail::Skeleton() {
    LOG_DEBUG("Create new Stencil object for Matrix (", this, ")");
    add(source, north, west, south, east, padding, neutral_element, func);
}

//Konstruktor für Vektor
template<typename Tin, typename Tout>
Stencil<Tout(Tin)>::Stencil(const Source& source, unsigned int west, unsigned int east,
                            detail::Padding padding, Tin neutral_element, const std::string& func):
    detail::Skeleton() {
    LOG_DEBUG("Create new Stencil object for Vector (", this, ")");
    add(source, 0, west, 0, east, padding, neutral_element, func);
}

template<typename Tin, typename Tout>
void Stencil<Tout(Tin)>::add(const Source& source, unsigned int north, unsigned int west, unsigned int south, unsigned int east,
         detail::Padding padding, Tin neutral_element, const std::string& func){
    LOG_DEBUG("A");
    StencilInfo<Tout(Tin)> info(source, north, west, south, east, padding, neutral_element, func);
    LOG_DEBUG("B");
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

        LOG_DEBUG("local: ", local[0], ",", local[1], " global: ", global[0],
                ",", global[1]);
        unsigned int i = 0;
        try {
            for(auto& sInfo : _stencilInfos){
                cl::Kernel kernel(sInfo.getProgram().kernel(*devicePtr, "SCL_STENCIL"));
                int k = 0;
                for(i = 0; i<_iterations; i++) {
                    int j = 0;
                    if((i+k) % 2 == 0){
                        kernel.setArg(j++, inputBuffer.clBuffer());
                        kernel.setArg(j++, outputBuffer.clBuffer());
                        kernel.setArg(j++, inputBuffer.clBuffer());
                        kernel.setArg(j++, sInfo.getTileWidth()*sInfo.getTileHeight()*sizeof(Tin), NULL);
                    } else {
                        kernel.setArg(j++, inputBuffer.clBuffer());
                        kernel.setArg(j++, inputBuffer.clBuffer());
                        kernel.setArg(j++, outputBuffer.clBuffer());
                        kernel.setArg(j++, sInfo.getTileWidth()*sInfo.getTileHeight()*sizeof(Tin), NULL);
                    }
                    kernel.setArg(j++, elements);   // elements
                    kernel.setArg(j++, sInfo.getNorth()); // north
                    kernel.setArg(j++, sInfo.getWest()); // west
                    kernel.setArg(j++, sInfo.getSouth()); // south
                    kernel.setArg(j++, sInfo.getEast()); // east
                    kernel.setArg(j++, determineLargestNorth());
                    kernel.setArg(j++, static_cast<cl_uint>(output.columnCount())); // number of columns

                    detail::kernelUtil::setKernelArgs(kernel, *devicePtr, j,
                            std::forward<Args>(args)...);

                    LOG_DEBUG(sInfo.getDebugInfo());

                    // keep buffers and arguments alive / mark them as in use
                    auto keepAlive = detail::kernelUtil::keepAlive(*devicePtr,
                            inputBuffer.clBuffer(), outputBuffer.clBuffer(), std::forward<Args>(args)...);

                    // after finishing the kernel invoke this function ...
                    auto invokeAfter = [=] () {(void)keepAlive;};
                    devicePtr->enqueue(kernel, cl::NDRange(global[0], global[1]),
                        cl::NDRange(local[0], local[1]), cl::NullRange, // offset
                        invokeAfter);
                    k++;
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
    in.setDistribution(detail::StencilDistribution<Matrix<Tin>>(determineLargestNorth(), determineLargestWest(), determineLargestSouth(), determineLargestEast(),
    		stencilInfo.getPadding(), stencilInfo.getNeutralElement()));

    // create buffers if required
    in.createDeviceBuffers();

    // copy data to devices
    in.startUpload();
}

template<typename Tin, typename Tout>
unsigned int Stencil<Tout(Tin)>::determineLargestNorth() {
    unsigned int largestNorth = 0;
    for(auto& s : _stencilInfos) {
        if(s.getNorth() > largestNorth){
            largestNorth = s.getNorth();
        }
    }
    return largestNorth;
}

template<typename Tin, typename Tout>
unsigned int Stencil<Tout(Tin)>::determineLargestWest() {
    unsigned int largestWest = 0;
    for(auto& s : _stencilInfos){
        if(s.getWest() > largestWest){
            largestWest = s.getWest();
        }
    }
    return largestWest;
}
template<typename Tin, typename Tout>
unsigned int Stencil<Tout(Tin)>::determineLargestSouth() {
    unsigned int largestSouth = 0;
    for(auto& s : _stencilInfos){
        if(s.getSouth() > largestSouth){
            largestSouth = s.getSouth();
        }
    }
    return largestSouth;
}
template<typename Tin, typename Tout>
unsigned int Stencil<Tout(Tin)>::determineLargestEast() {
    unsigned int largestEast = 0;
    for(auto& s : _stencilInfos){
        if(s.getEast() > largestEast){
            largestEast = s.getEast();
        }
    }
    return largestEast;
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
