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
/// Works with the matrix.
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
Stencil<Tout(Tin)>::Stencil(const Source& source, unsigned int north,
		unsigned int west, unsigned int south, unsigned int east,
		detail::Padding padding, Tin neutral_element, const std::string& func,
		int iterBetSwaps) :
		detail::Skeleton(), _iterBetSwaps(iterBetSwaps) {
	LOG_DEBUG_INFO("Create new Stencil object for Matrix (", this, ")");
	add(source, north, west, south, east, padding, neutral_element, func);
}

//Konstruktor für Matrix (Convenience for square stencil shapes)
template<typename Tin, typename Tout>
Stencil<Tout(Tin)>::Stencil(const Source& source, unsigned int range,
		detail::Padding padding, Tin neutral_element, const std::string& func,
		int iterBetSwaps) :
		detail::Skeleton(), _iterBetSwaps(iterBetSwaps) {
	LOG_DEBUG_INFO("Create new Stencil object for Matrix (", this, ")");
	add(source, range, range, range, range, padding, neutral_element, func);
}

//Konstruktor für Vektor
template<typename Tin, typename Tout>
Stencil<Tout(Tin)>::Stencil(const Source& source, unsigned int west,
		unsigned int east, detail::Padding padding, Tin neutral_element,
		const std::string& func, int iterBetSwaps) :
		detail::Skeleton(), _iterBetSwaps(iterBetSwaps) {
	LOG_DEBUG_INFO("Create new Stencil object for Vector (", this, ")");
	add(source, 0, west, 0, east, padding, neutral_element, func);
}

template<typename Tin, typename Tout>
void Stencil<Tout(Tin)>::add(const Source& source, unsigned int range,
		detail::Padding padding, Tin neutral_element, const std::string& func) {
	add(source, range, range, range, range, padding, neutral_element, func);
}

template<typename Tin, typename Tout>
void Stencil<Tout(Tin)>::add(const Source& source, unsigned int north,
		unsigned int west, unsigned int south, unsigned int east,
		detail::Padding padding, Tin neutral_element, const std::string& func) {
    _stencilInfos.emplace_back(source, north, west, south, east, padding,
                               neutral_element, func);
}

// Ausführungsoperator
template<typename Tin, typename Tout>
template<typename ... Args>
Matrix<Tout> Stencil<Tout(Tin)>::operator()(unsigned int iterations,
		const Matrix<Tin>& in, Args&&... args) {
	Matrix<Tout> output;
	this->operator()(iterations, out(output), in, std::forward<Args>(args)...);
	return output;
}

// Ausführungsoperator mit Referenz. Kapselt User vom Vorhandensein einer temp-Matrix.
template<typename Tin, typename Tout>
template<typename ... Args>
Matrix<Tout>& Stencil<Tout(Tin)>::operator()(unsigned int iterations,
		Out<Matrix<Tout>> output, const Matrix<Tin>& in, Args&&... args) {
	Matrix<Tout> temp;
	return this->operator()(iterations, output, out(temp), in,
			std::forward<Args>(args)...);
}

// Ausführungsoperator mit Referenz, der zusätzlich noch die temp-Matrix aufnimmt
template<typename Tin, typename Tout>
template<typename ... Args>
Matrix<Tout>& Stencil<Tout(Tin)>::operator()(unsigned int iterations,
		Out<Matrix<Tout>> output, Out<Matrix<Tout>> temp, const Matrix<Tin>& in,
		Args&&... args) {
	ASSERT(iterations > 0);
	ASSERT(in.rowCount() > 0);
	ASSERT(in.columnCount() > 0);

	_iterations = iterations;
	prepareInput(in);

	prepareAdditionalInput(std::forward<Args>(args)...);

	prepareOutput(output.container(), in);
	prepareOutput(temp.container(), in);

	//Wann muss die temp-Matrix und wann die reguläre output-Matrix zurückgegeben werden?:
	//Wird eine gerade Anzahl Iterationen durchgeführt, so muss immer die temp-Matrix benutzt werden.
	//Bei ungerader Anzahl Iterationen hängt es von der Anzahl der verwendeten Stencil Shapes ab.
	//Da das Kopieren und Zuweisen von Matrizen hier explizit verboten wurde, wird hier im vorhinein die Ausgabe-Matrix bestimmt.
	if (_iterations % 2 == 0) {
		execute(temp.container(), output.container(), in,
				std::forward<Args>(args)...);
	} else {
		if ((_iterations % 2 == 1) && (_stencilInfos.size() % 2 == 0)) {
			execute(temp.container(), output.container(), in,
					std::forward<Args>(args)...);
		} else {
			execute(output.container(), temp.container(), in,
					std::forward<Args>(args)...);
		}
	}

	updateModifiedStatus(temp, output, std::forward<Args>(args)...);

	return output.container();
}

// Ausführen
template<typename Tin, typename Tout>
template<typename ... Args>
void Stencil<Tout(Tin)>::execute(Matrix<Tout>& output, Matrix<Tout>& temp,
		const Matrix<Tin>& in, Args&&... args) {
	ASSERT(in.distribution().isValid());
	ASSERT(
			output.rowCount() == in.rowCount()
					&& output.columnCount() == in.columnCount());
	unsigned int iterationsBetweenSwaps = determineIterationsBetweenDataSwaps(
			in, _iterations);
	unsigned int iterationsAfterLastSync = 0;

	unsigned int outputRowCount = output.rowCount();
	unsigned int noOfDevices = in.distribution().devices().size();

	unsigned int southSum = determineSouthSum();
	unsigned int northSum = determineNorthSum();
	unsigned int i = 0;
	int k = 1;

	try {
		for (i = 0; i < _iterations; i++) {
			k--;
			if (noOfDevices != 1
					&& iterationsAfterLastSync == iterationsBetweenSwaps ) {
				if ((i + k) % 2 == 0) {
					(dynamic_cast<detail::StencilDistribution<Matrix<Tout> >*>(&temp.distribution()))->swap(
							temp, iterationsBetweenSwaps);
				} else if ((i + k) % 2 == 1) {
					(dynamic_cast<detail::StencilDistribution<Matrix<Tout> >*>(&output.distribution()))->swap(
							output, iterationsBetweenSwaps);
				}
				iterationsBetweenSwaps = determineIterationsBetweenDataSwaps(in,
						_iterations - i - 1);
				iterationsAfterLastSync = 0;
			}

			for (auto& sInfo : _stencilInfos) {
				for (auto& devicePtr : in.distribution().devices()) {
					auto& outputBuffer = output.deviceBuffer(*devicePtr);
					auto& inputBuffer = in.deviceBuffer(*devicePtr);
					auto& tempBuffer = temp.deviceBuffer(*devicePtr);

					cl_uint elements = static_cast<cl_uint>(inputBuffer.size());

					cl::Kernel kernel(
							sInfo.getProgram().kernel(*devicePtr,
									"SCL_STENCIL"));

					cl_uint workgroupSize =
							static_cast<cl_uint>(detail::kernelUtil::determineWorkgroupSizeForKernel(
									kernel, *devicePtr));
LOG_INFO("workgroupSize: ", workgroupSize);
workgroupSize = 256;
					cl_uint offset = 0;
					//cl_uint local[2] = { static_cast<cl_uint>(sqrt(
					//		workgroupSize)), static_cast<cl_uint>(local[0]) };
cl_uint local[2] = { 32, 4 };
					cl_uint global[2] = {
							static_cast<cl_uint>(detail::util::ceilToMultipleOf(
									output.columnCount(), local[0])),
							static_cast<cl_uint>(detail::util::ceilToMultipleOf(
									output.rowCount(), local[1])) }; // HALO
					if (devicePtr->id() == 0 && noOfDevices > 1) {
						global[1] =
								static_cast<cl_uint>(detail::util::ceilToMultipleOf(
										outputRowCount
												+ (iterationsBetweenSwaps
														- iterationsAfterLastSync)
														* southSum, local[1]));
					} else if (devicePtr->id() == noOfDevices - 1
							&& noOfDevices > 1) {
						offset = iterationsAfterLastSync * northSum;
						global[1] =
								static_cast<cl_uint>(detail::util::ceilToMultipleOf(
										outputRowCount
												+ (iterationsBetweenSwaps
														- iterationsAfterLastSync)
														* northSum, local[1]));
					} else if (noOfDevices > 1) {
						offset = iterationsAfterLastSync * northSum;
						global[1] =
								static_cast<cl_uint>(detail::util::ceilToMultipleOf(
										outputRowCount
												+ (iterationsBetweenSwaps
														- iterationsAfterLastSync)
														* northSum
												+ (iterationsBetweenSwaps
														- iterationsAfterLastSync)
														* southSum, local[1]));
					}
					//LOG_DEBUG_INFO("device: ", devicePtr->id(), " elements: ", elements);
					//Get time
					int j = 0;

					unsigned int tile_width = local[0]
							+ sInfo.getWest() + sInfo.getEast();
					unsigned int tile_height = local[1]
							+ sInfo.getNorth() + sInfo.getSouth();
					kernel.setArg(j++, inputBuffer.clBuffer());
					if ((i + k) == 0) {
						//Erste Iteration: Lese von input, schreibe zu Output
						kernel.setArg(j++, outputBuffer.clBuffer());
						kernel.setArg(j++, inputBuffer.clBuffer());
					} else if ((i + k) % 2 == 0) {
						//"Gerade" Iterationen+StencilShape: Lese von temp, schreibe zu Output
						kernel.setArg(j++, outputBuffer.clBuffer());
						kernel.setArg(j++, tempBuffer.clBuffer());
					} else if ((i + k) % 2 == 1) {
						//"Ungerade" Iterationen+StencilShape: Lese von Output, schreibe zu temp. Ist dies die letzte Iteration, muss temp zurückgegeben werden
						kernel.setArg(j++, tempBuffer.clBuffer());
						kernel.setArg(j++, outputBuffer.clBuffer());
					}
LOG_INFO("Allocate: ", tile_width * tile_height * sizeof(Tin), " bytes local memory");
					kernel.setArg(j++, tile_width * tile_height * sizeof(Tin),
							NULL); //Alloziere Local Memory
					kernel.setArg(j++, elements);   // elements
					kernel.setArg(j++,
							static_cast<cl_uint>(output.columnCount())); // number of columns

					detail::kernelUtil::setKernelArgs(kernel, *devicePtr, j,
							std::forward<Args>(args)...);

					auto keepAliveBuffer = outputBuffer.clBuffer();
					//Setze keepAlive-Buffer auf den Buffer, der die aktuellesten Daten enthält
					if ((i + k) % 2 == 1) {
						keepAliveBuffer = tempBuffer.clBuffer();
					}

					// keep buffers and arguments alive / mark them as in use
					auto keepAlive = detail::kernelUtil::keepAlive(*devicePtr,
							inputBuffer.clBuffer(), keepAliveBuffer,
							std::forward<Args>(args)...);

					// after finishing the kernel invoke this function ...
					auto invokeAfter = [=] () {(void)keepAlive;};
					if (devicePtr->id() == 0)
					{
						LOG_INFO("starting Stencil with ", global[0], "x", global[1], " - ", local[0], "x", local[1]);
						auto event = devicePtr->enqueue(kernel,
								cl::NDRange(global[0], global[1]),
								cl::NDRange(local[0], local[1]),
								cl::NDRange(0, offset), invokeAfter);
						event.wait(); 
						cl_int err = 0; 
						auto start = event.template getProfilingInfo<CL_PROFILING_COMMAND_START>(&err); 
						auto end = event.template getProfilingInfo<CL_PROFILING_COMMAND_END>(&err);
						LOG_INFO("kernel execution: ", (end - start) / 1e6, "ms");
					}
					else
						devicePtr->enqueue(kernel,
								cl::NDRange(global[0], global[1]),
								cl::NDRange(local[0], local[1]), cl::NullRange,
								invokeAfter);
				}

				k++;
			}
			iterationsAfterLastSync++;
			LOG_DEBUG_INFO("Stencil kernel ", i, " started");
		}

	} catch (cl::Error& err) {
		ABORT_WITH_ERROR(err);
	}
}

// Eingabe vorbereiten
template<typename Tin, typename Tout>
void Stencil<Tout(Tin)>::prepareInput(const Matrix<Tin>& in) {
	// set distribution
	detail::StencilDistribution<Matrix<Tin>> dist(determineNorthSum(),
			determineWestSum(), determineSouthSum(), determineEastSum(),
			determineIterationsBetweenDataSwaps(in, _iterations));
	in.setDistribution(dist);

	// create buffers if required
	in.createDeviceBuffers();

	// copy data to devices
	in.startUpload();
}

template<typename Tin, typename Tout>
unsigned int Stencil<Tout(Tin)>::determineIterationsBetweenDataSwaps(
        const Matrix<Tin>& /*in*/, unsigned int iterLeft) {
	//User chose a value
	if (iterLeft <= _iterBetSwaps)
		return iterLeft;
	else if (_iterBetSwaps != -1)
		return _iterBetSwaps;

	//Add the online determination of the number of iterations between device synchronizations here

	return 1;
}

template<typename Tin, typename Tout>
unsigned int Stencil<Tout(Tin)>::determineNorthSum() {
	unsigned int largestNorth = 0;
	for (auto& s : _stencilInfos) {
		largestNorth += s.getNorth();
	}
	return largestNorth;
}

template<typename Tin, typename Tout>
unsigned int Stencil<Tout(Tin)>::determineWestSum() {
	unsigned int largestWest = 0;
	for (auto& s : _stencilInfos) {
		largestWest += s.getWest();
	}
	return largestWest;
}
template<typename Tin, typename Tout>
unsigned int Stencil<Tout(Tin)>::determineSouthSum() {
	unsigned int largestSouth = 0;
	for (auto& s : _stencilInfos) {
		largestSouth += s.getSouth();
	}
	return largestSouth;
}
template<typename Tin, typename Tout>
unsigned int Stencil<Tout(Tin)>::determineEastSum() {
	unsigned int largestEast = 0;
	for (auto& s : _stencilInfos) {
		largestEast += s.getEast();
	}
	return largestEast;
}

// Ausgabe vorbereiten
template<typename Tin, typename Tout>
void Stencil<Tout(Tin)>::prepareOutput(Matrix<Tout>& output,
		const Matrix<Tin>& in) {
	// set size
	if (output.rowCount() != in.rowCount())
		output.resize(
				typename Matrix<Tout>::size_type(in.rowCount(),
						in.columnCount()));

	// adopt distribution from in input
	output.setDistribution(in.distribution()); // richtiger typ (Tout)?

	//create buffers if required
	output.createDeviceBuffers();
}

}

#endif /* STENCILDEF_H_ */

