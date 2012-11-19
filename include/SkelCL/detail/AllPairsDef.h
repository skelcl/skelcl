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
/// \file AllPairsDef.h
///
///	\author Malte Friese <malte.friese@uni-muenster.de>
///

#ifndef ALLPAIRS_DEF_H
#define ALLPAIRS_DEF_H

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

#include "../Distributions.h"
#include "../Matrix.h"
#include "../Reduce.h"
#include "../Zip.h"
#include "../Out.h"

#include "Assert.h"
#include "Device.h"
#include "KernelUtil.h"
#include "Logger.h"
#include "Program.h"
#include "Skeleton.h"
#include "Util.h"

namespace skelcl {

// Konstruktor
template<typename Tleft, typename Tright, typename Tout>
AllPairs<Tout(Tleft, Tright)>::AllPairs(const Reduce<Tout(Tout)>& reduce, const Zip<Tout(Tleft, Tright)>& zip)
    : detail::Skeleton(),
      _srcReduce(reduce._source),
      _srcZip(zip._source)
{
    LOG_DEBUG("Create new AllPairs object (", this, ")");
}

// Ausführungsoperator
template<typename Tleft, typename Tright, typename Tout>
template <typename... Args>
Matrix<Tout> AllPairs<Tout(Tleft, Tright)>::operator()(const Matrix<Tleft>& left,
                                                       const Matrix<Tright>& right,
                                                       Args&&... args)
{
    Matrix<Tout> output;
    this->operator()(out(output), left, right, std::forward<Args>(args)...);
    return output;
}

// Ausführungsoperator mit Referenz
template<typename Tleft, typename Tright, typename Tout>
template <typename... Args>
Matrix<Tout>& AllPairs<Tout(Tleft, Tright)>::operator()(Out< Matrix<Tout> > output,
                                                        const Matrix<Tleft>& left,
                                                        const Matrix<Tright>& right,
                                                        Args&&... args)
{
    ASSERT( left.columnCount() == right.rowCount() );

    auto program = createAndBuildProgram<Matrix>();

    prepareInputInput(left, right);

    prepareOutput(output.container(), left, right);

    execute(program, output.container(), left, right, std::forward<Args>(args)...);

    updateModifiedStatus(output, std::forward<Args>(args)...);

    return output.container();
}

// Ausführen
template<typename Tleft, typename Tright, typename Tout>
template <typename... Args>
void AllPairs<Tout(Tleft, Tright)>::execute(const detail::Program& program,
                                            Matrix<Tout>& output,
                                            const Matrix<Tleft>& left,
                                            const Matrix<Tright>& right,
                                            Args&&... args)
{
    ASSERT( left.distribution().isValid() && right.distribution().isValid() );
    ASSERT( output.columnCount() == left.rowCount() && output.rowCount() == right.columnCount() );

    for (auto& devicePtr : left.distribution().devices()) { // ab hier neu
        auto& outputBuffer = output.deviceBuffer(*devicePtr);
        auto& leftBuffer   = left.deviceBuffer(*devicePtr);
        auto& rightBuffer  = right.deviceBuffer(*devicePtr);

        cl_uint elements[2]   = {output.rowCount(), output.columnCount()};
        cl_uint local[2]      = {16, 32}; // richtig rum?
        cl_uint global[2]     = {detail::util::ceilToMultipleOf(elements[0], local[0]), // durch SUBTILES teilen
                                 detail::util::ceilToMultipleOf(elements[1], local[1])};

        try {
            cl::Kernel kernel(program.kernel(*devicePtr, "SCL_ALLPAIRS"));

            kernel.setArg(0, leftBuffer.clBuffer());
            kernel.setArg(1, rightBuffer.clBuffer());
            kernel.setArg(2, outputBuffer.clBuffer());
            kernel.setArg(3, elements[0]); // height
            kernel.setArg(4, elements[1]); // width

            detail::kernelUtil::setKernelArgs(kernel, *devicePtr, 5, std::forward<Args>(args)...);

            auto invokeAfter = [&] ()  {
                                 leftBuffer.markAsNotInUse();
                                 rightBuffer.markAsNotInUse();
                                 outputBuffer.markAsNotInUse();
                               };

            devicePtr->enqueue(kernel, cl::NDRange(global[0], global[1]), cl::NDRange(local[0], local[1]),
                               cl::NullRange, // offset
                               invokeAfter);

            // after successfully enqueued the kernel
            leftBuffer.markAsInUse();
            rightBuffer.markAsInUse();
            outputBuffer.markAsInUse();

        } catch (cl::Error& err) {
            ABORT_WITH_ERROR(err);
        }
    }
    LOG_INFO("AllPairs kernel started");
}

// Programm erstellen
template<typename Tleft, typename Tright, typename Tout>
detail::Program AllPairs<Tout(Tleft, Tright)>::createAndBuildProgram() const
{
    ASSERT_MESSAGE( !_srcReduce.isEmpty(),
                    "Tried to create program with empty user reduce source." );
    ASSERT_MESSAGE( !_srcZip.isEmpty(),
                    "Tried to create program with empty user zip source." );

    // create program
    std::string s(Matrix<Tleft>::deviceFunctions()); // left?

    // reduce user source
    s.append(_srcReduce); // umbennnen

    // zip user source
    s.append(_srcZip);

    // allpairs skeleton source
    s.append( #include "AllPairsKernel.cl" );

    auto program = detail::Program(s, detail::util::hash("//AllPairs\n"
                                                         + Matrix<Tleft>::deviceFunctions()
                                                         + _srcReduce
                                                         + _srcZip));
    // modify program
    if (!program.loadBinary()) {
        // TODO
    }

    program.build();

    return program;
}

// Einagbe vorbereiten
template<typename Tleft, typename Tright, typename Tout>
void AllPairs<Tout(Tleft, Tright)>::prepareInput(const Matrix<Tleft>& left,
                                                 const Matrix<Tright>& right)
{
    //TODO verteilungen einstellen, buffer erzeugen, daten uploaden
}

// Ausgabe vorbereiten
template<typename Tleft, typename Tright, typename Tout>
void llPairs<Tout(Tleft, Tright)>::prepareOutput(Matrix<Tout>& output,
                                                 const Matrix<Tleft>& left,
                                                 const Matrix<Tright>& right)
{
    //TODO groesse bestimmen, verteilung angeben, buffer erzeugen
}

} // namespace skelcl

#endif // ALLPAIRS_DEF_H
