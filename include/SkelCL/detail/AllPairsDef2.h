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
/// \file AllPairsDef2.h
///
///	\author Malte Friese <malte.friese@uni-muenster.de>
///

#ifndef ALLPAIRS2_DEF_H
#define ALLPAIRS2_DEF_H

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
AllPairs2<Tout(Tleft, Tright)>::AllPairs2(const std::string& source, const std::string& func)
    : detail::Skeleton(),
      _srcUser(source),
      _funcUser(func)
{
    LOG_DEBUG("Create new AllPairs2 object (", this, ")");
}

// Ausführungsoperator
template<typename Tleft, typename Tright, typename Tout>
template <typename... Args>
Matrix<Tout> AllPairs2<Tout(Tleft, Tright)>::operator()(const Matrix<Tleft>& left,
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
Matrix<Tout>& AllPairs2<Tout(Tleft, Tright)>::operator()(Out< Matrix<Tout> > output,
                                                        const Matrix<Tleft>& left,
                                                        const Matrix<Tright>& right,
                                                        Args&&... args)
{
    ASSERT( (left.rowCount() > 0) && (right.columnCount() > 0) );
    ASSERT( left.columnCount() == right.rowCount() );
    ASSERT( left.columnCount() > 0 );

    detail::Program program = createAndBuildProgram();

    prepareInput(left, right);

    prepareAdditionalInput(std::forward<Args>(args)...);

    prepareOutput(output.container(), left, right);

    execute(program, output.container(), left, right, std::forward<Args>(args)...);

    updateModifiedStatus(output, std::forward<Args>(args)...);

    return output.container();
}

// Ausführen
template<typename Tleft, typename Tright, typename Tout>
template <typename... Args>
void AllPairs2<Tout(Tleft, Tright)>::execute(const detail::Program& program,
                                            Matrix<Tout>& output,
                                            const Matrix<Tleft>& left,
                                            const Matrix<Tright>& right,
                                            Args&&... args)
{
    ASSERT( left.distribution().isValid() && right.distribution().isValid() );
    ASSERT( output.rowCount() == left.rowCount() && output.columnCount() == right.columnCount() );

    for (auto& devicePtr : left.distribution().devices()) {
        auto& outputBuffer = output.deviceBuffer(*devicePtr);
        auto& leftBuffer   = left.deviceBuffer(*devicePtr);
        auto& rightBuffer  = right.deviceBuffer(*devicePtr);

        cl_uint elements[2]   = {output.rowCount(), output.columnCount()};
        cl_uint local[2]      = {16, 16};
        cl_uint global[2]     = {detail::util::ceilToMultipleOf(elements[1], local[0]),
                                 detail::util::ceilToMultipleOf(elements[0], local[1])};
        cl_uint dimension     = left.columnCount();

        LOG_DEBUG("dim: ", dimension, " height: ", elements[0], " width: ",elements[1]);
        LOG_DEBUG("local: ", local[0],",", local[1], " global: ", global[0],",",global[1]);

        try {
            cl::Kernel kernel(program.kernel(*devicePtr, "SCL_ALLPAIRS2"));

            kernel.setArg(0, leftBuffer.clBuffer());
            kernel.setArg(1, rightBuffer.clBuffer());
            kernel.setArg(2, outputBuffer.clBuffer());
            kernel.setArg(3, dimension);   // dimension
            kernel.setArg(4, elements[0]); // height
            kernel.setArg(5, elements[1]); // width

            detail::kernelUtil::setKernelArgs(kernel, *devicePtr, 6,
                                              std::forward<Args>(args)...);


            // keep buffers and arguments alive / mark them as in use
            auto keepAlive = detail::kernelUtil::keepAlive(*devicePtr,
                                                           leftBuffer.clBuffer(),
                                                           rightBuffer.clBuffer(),
                                                           outputBuffer.clBuffer(),
                                                           std::forward<Args>(args)...);

            // after finishing the kernel invoke this function ...
            auto invokeAfter =  [=] () { (void)keepAlive; };

            devicePtr->enqueue(kernel, cl::NDRange(global[0], global[1]), cl::NDRange(local[0], local[1]),
                               cl::NullRange, // offset
                               invokeAfter);

        } catch (cl::Error& err) {
            ABORT_WITH_ERROR(err);
        }
    }
    LOG_INFO("AllPairs2 kernel started");
}

// Programm erstellen
template<typename Tleft, typename Tright, typename Tout>
detail::Program AllPairs2<Tout(Tleft, Tright)>::createAndBuildProgram() const
{
    ASSERT_MESSAGE( !_srcUser.empty(),
                    "Tried to create program with empty user source." );

    // create program
    std::string s(Matrix<Tout>::deviceFunctions());

    // helper structs and functions
    s.append(R"(
typedef float SCL_TYPE_0;
typedef float SCL_TYPE_1;
typedef float SCL_TYPE_2;

typedef struct {
    const __global SCL_TYPE_0* data;
    unsigned int dimension;
    unsigned int row;
} lmatrix_t;

typedef struct {
    const __global SCL_TYPE_1* data;
    unsigned int width;
    unsigned int column;
} rmatrix_t;

SCL_TYPE_0 getElementFromRow(lmatrix_t *matrix, const unsigned int element_id) {
    return matrix->data[(matrix->row) * matrix->dimension + element_id];
}

SCL_TYPE_1 getElementFromColumn(rmatrix_t *matrix, const unsigned int element_id) {
    return matrix->data[element_id * (matrix->width) + matrix->column];
}

)");

    // user source
    s.append(_srcUser);

    // allpairs skeleton source
    s.append(
      #include "AllPairsKernel2.cl"
    );

    auto program = detail::Program(s, detail::util::hash("//AllPairs2\n"
                                                         + Matrix<Tout>::deviceFunctions()
                                                         + _srcUser
                                                         + _funcUser));
    // modify program
    if (!program.loadBinary()) {
        program.transferParameters(_funcUser, 3, "SCL_ALLPAIRS2");
        program.transferArguments(_funcUser, 3, "USR_FUNC");

        program.renameFunction(_funcUser, "USR_FUNC");

        program.adjustTypes<Tleft, Tright, Tout>();
    }

    program.build();

    return program;
}

// Eingabe vorbereiten
template<typename Tleft, typename Tright, typename Tout>
void AllPairs2<Tout(Tleft, Tright)>::prepareInput(const Matrix<Tleft>& left,
                                                 const Matrix<Tright>& right)
{
    bool isLeftCopy    = (dynamic_cast<detail::CopyDistribution< Matrix<Tleft> >*>(&left.distribution())     != nullptr);
    bool isLeftSingle  = (dynamic_cast<detail::SingleDistribution< Matrix<Tleft> >*>(&left.distribution())   != nullptr);
    bool isLeftBlock   = (dynamic_cast<detail::BlockDistribution< Matrix<Tleft> >*>(&left.distribution())    != nullptr);

    bool isRightCopy   = (dynamic_cast<detail::CopyDistribution< Matrix<Tright> >*>(&right.distribution())   != nullptr);
    bool isRightSingle = (dynamic_cast<detail::SingleDistribution< Matrix<Tright> >*>(&right.distribution()) != nullptr);
    bool isRightBlock  = (dynamic_cast<detail::BlockDistribution< Matrix<Tright> >*>(&right.distribution())  != nullptr);

    // set distributions

    // left and right not valid
    if (!left.distribution().isValid() && !right.distribution().isValid()) {
        left.setDistribution(detail::BlockDistribution< Matrix<Tleft> >());
        right.setDistribution(detail::CopyDistribution< Matrix<Tright> >());

    // only left not valid
    } else if (!left.distribution().isValid()) {
        if (isRightCopy) // right copy -> left block
            left.setDistribution(detail::BlockDistribution< Matrix<Tleft> >());
        else if (isRightSingle) // right single -> left single
            left.setDistribution(detail::SingleDistribution< Matrix<Tleft> >());
        else if (isRightBlock) { // right block -> left block + change right to copy
            left.setDistribution(detail::BlockDistribution< Matrix<Tleft> >());
            right.setDistribution(detail::CopyDistribution< Matrix<Tright> >());
        }
        else
            ASSERT_MESSAGE(false, "if check only valid for single, copy and block distribution. new distribution added?");

    // only right not valid
    } else if (!right.distribution().isValid()) {
        if (isLeftBlock) // left block -> right copy
            right.setDistribution(detail::CopyDistribution< Matrix<Tright> >());
        else if (isLeftCopy) // left copy -> right copy
            right.setDistribution(detail::CopyDistribution< Matrix<Tright> >());
        else if (isLeftSingle) // left single -> right single
            right.setDistribution(detail::SingleDistribution< Matrix<Tright> >());
        else
            ASSERT_MESSAGE(false, "if check only valid for single, copy and block distribution. new distribution added?");

    // both valid
    } else {
        if (isLeftCopy && isRightSingle)
            left.setDistribution(detail::SingleDistribution< Matrix<Tleft> >()); // single okay?
        else if (isLeftBlock && isRightSingle)
            right.setDistribution(detail::CopyDistribution< Matrix<Tright> >());
        else if (isLeftSingle && isRightBlock)
            right.setDistribution(detail::SingleDistribution< Matrix<Tright> >()); // single okay?
        else if (isLeftCopy && isRightBlock)
            right.setDistribution(detail::CopyDistribution< Matrix<Tright> >());
        else if (isLeftBlock && isRightBlock)
            right.setDistribution(detail::CopyDistribution< Matrix<Tright> >());
    }

    // create buffers if required
    left.createDeviceBuffers();
    right.createDeviceBuffers();

    // copy data to devices
    left.startUpload();
    right.startUpload();
}

// Ausgabe vorbereiten
template<typename Tleft, typename Tright, typename Tout>
void AllPairs2<Tout(Tleft, Tright)>::prepareOutput(Matrix<Tout>& output,
                                                  const Matrix<Tleft>& left,
                                                  const Matrix<Tright>& right)
{
    // set size
    if (output.rowCount() != left.rowCount() || output.columnCount() != right.columnCount())
        output.resize(typename Matrix<Tout>::size_type(left.rowCount(), right.columnCount()));

    // adopt distribution from left input
    output.setDistribution(left.distribution()); // richtiger typ (Tout)?

    //create buffers if required
    output.createDeviceBuffers();
}

} // namespace skelcl

#endif // ALLPAIRS2_DEF_H
