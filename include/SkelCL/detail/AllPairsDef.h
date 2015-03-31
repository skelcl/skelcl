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

//#include <ssedit/TempSourceFile.h>
//#include <ssedit/Function.h>

#include <pvsutil/Assert.h>
#include <pvsutil/Logger.h>
#include <pvsutil/Timer.h>

#include <stooling/SourceCode.h>

#include "../Distributions.h"
#include "../Matrix.h"
#include "../Reduce.h"
#include "../Zip.h"
#include "../Out.h"

#include "Device.h"
#include "KernelUtil.h"
#include "Program.h"
#include "Skeleton.h"
#include "Util.h"

#define KNOB_C 32
#define KNOB_R 8
#define KNOB_S 16

#define GENERIC_KNOB_C 16
#define GENERIC_KNOB_R 16
#define GENERIC_KNOB_S 1

namespace skelcl {

template<typename Tleft, typename Tright, typename Tout>
AllPairs<Tout(Tleft, Tright)>::AllPairs(const Reduce<Tout(Tout)>& reduce, const Zip<Tout(Tleft, Tright)>& zip)
    : detail::Skeleton("AllPairs<Tout(Tleft, Tright)>"),
      _srcReduce(reduce.source()),
      _srcZip(zip.source()),
      _funcReduce(reduce.func()),
      _funcZip(zip.func()),
      _idReduce(reduce.id()),
      _srcUser(),
      _funcUser(),
      _C(KNOB_C), _R(KNOB_R), _S(KNOB_S), // parameters
      _program(createAndBuildProgramSpecial())
{
    LOG_DEBUG("Create new AllPairs object (", this, ")");
}

template<typename Tleft, typename Tright, typename Tout>
AllPairs<Tout(Tleft, Tright)>::AllPairs(const std::string& source, const std::string& func)
    : detail::Skeleton("AllPairs<Tout(Tleft, Tright)>"),
      _srcReduce(),
      _srcZip(),
      _funcReduce(),
      _funcZip(),
      _idReduce(),
      _srcUser(source),
      _funcUser(func),
      _C(GENERIC_KNOB_C), _R(GENERIC_KNOB_R), _S(GENERIC_KNOB_S),
      _program(createAndBuildProgramGeneral())
{
    LOG_DEBUG("Create new AllPairs object (", this, ")");
}

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

template<typename Tleft, typename Tright, typename Tout>
template <typename... Args>
Matrix<Tout>& AllPairs<Tout(Tleft, Tright)>::operator()(Out< Matrix<Tout> > output,
                                                        const Matrix<Tleft>& left,
                                                        const Matrix<Tright>& right,
                                                        Args&&... args)
{
    ASSERT( (left.rowCount() > 0) && (right.columnCount() > 0) );
    ASSERT( left.columnCount() == right.rowCount() );
    ASSERT( left.columnCount() > 0 );

    pvsutil::Timer t; // Time how long it takes to prepare input and output data.

    prepareInput(left, right);
    prepareAdditionalInput(std::forward<Args>(args)...);
    prepareOutput(output.container(), left, right);

    // Profiling information.
    LOG_PROF(_name, "[", this, "] prepare ", t.stop(), " ms");

    execute(output.container(), left, right, std::forward<Args>(args)...);

    updateModifiedStatus(output, std::forward<Args>(args)...);

    return output.container();
}

template<typename Tleft, typename Tright, typename Tout>
template <typename... Args>
void AllPairs<Tout(Tleft, Tright)>::execute(Matrix<Tout>& output,
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

        cl_uint elements[2]   = { static_cast<cl_uint>(
                                    outputBuffer.size() / output.columnCount()),
                                  static_cast<cl_uint>(output.columnCount()) };
        cl_uint local[2]      = {_C, _R}; // C, R
        cl_uint global[2]     = {static_cast<cl_uint>(
                                  detail::util::ceilToMultipleOf(elements[1],
                                                                 local[0])),
                                 static_cast<cl_uint>(
                                  detail::util::ceilToMultipleOf(elements[0],
                                                                 local[1]*_S))
                                   /_S}; // SUBTILES
        cl_uint dimension     = static_cast<cl_uint>( left.columnCount() );

        LOG_DEBUG("dim: ", dimension, " height: ",
                  elements[0], " width: ",elements[1]);
        LOG_DEBUG("local: ", local[0],",", local[1],
                  " global: ", global[0],",",global[1]);

        try {
            cl::Kernel kernel(_program.kernel(*devicePtr, "SCL_ALLPAIRS"));

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

            _events.push_back(devicePtr->enqueue(kernel,
                                                 cl::NDRange(global[0], global[1]),
                                                 cl::NDRange(local[0], local[1]),
                                                 cl::NullRange, // offset
                                                 invokeAfter));
        } catch (cl::Error& err) {
            ABORT_WITH_ERROR(err);
        }
    }
    LOG_INFO("AllPairs kernel started");
}

template<typename Tleft, typename Tright, typename Tout>
detail::Program AllPairs<Tout(Tleft, Tright)>::createAndBuildProgramSpecial() const
{
    ASSERT_MESSAGE( !_srcReduce.empty(),
                    "Tried to create program with empty user reduce source." );
    ASSERT_MESSAGE( !_srcZip.empty(),
                    "Tried to create program with empty user zip source." );

    // _srcReduce: replace func by TMP_REDUCE
    stooling::SourceCode rSource(_srcReduce);
    rSource.renameFunction(_funcReduce, "TMP_REDUCE");

    // _srcZip: replace func by TMP_ZIP
    stooling::SourceCode zSource(_srcZip);
    zSource.renameFunction(_funcReduce, "TMP_ZIP");

    // create program
    std::string s(Matrix<Tout>::deviceFunctions());

    // identity
    s.append("#define SCL_IDENTITY ").append(_idReduce);

    s.append("\n");

    // reduce user source
    s.append(rSource.code());

    s.append("\n");

    // zip user source
    s.append(zSource.code());

    s.append("\n");

    // allpairs parameters
    s.append("#define C ").append(std::to_string(_C)).append("\n");
    s.append("#define R ").append(std::to_string(_R)).append("\n");
    s.append("#define S ").append(std::to_string(_S)).append("\n");
    s.append("#define D 32");

    // allpairs skeleton source
    s.append(
      #include "AllPairsKernel.cl"
    );

    auto program = detail::Program(s, detail::util::hash("//AllPairs\n"
                                                         + Matrix<Tout>::deviceFunctions()
                                                         + _idReduce
                                                         + rSource.code()
                                                         + zSource.code()));
    // modify program
    if (!program.loadBinary()) {
        // problem: reduce parameter a und zip parameter a
        program.transferParameters("TMP_REDUCE", 2, "SCL_ALLPAIRS"); 
        program.transferArguments("TMP_REDUCE", 2, "USR_REDUCE");
        // TODO: Order? first args from reduce than zip??
        program.transferParameters("TMP_ZIP", 2, "SCL_ALLPAIRS");
        program.transferArguments("TMP_ZIP", 2, "USR_ZIP");

        program.renameFunction("TMP_REDUCE", "USR_REDUCE");
        program.renameFunction("TMP_ZIP", "USR_ZIP");

        program.adjustTypes<Tleft, Tright, Tout>();
    }

    program.build();

    return program;
}

template<typename Tleft, typename Tright, typename Tout>
detail::Program AllPairs<Tout(Tleft, Tright)>::createAndBuildProgramGeneral() const
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

SCL_TYPE_0 getElementFromRow(lmatrix_t* matrix, const unsigned int element_id) {
    return matrix->data[(matrix->row) * matrix->dimension + element_id];
}

SCL_TYPE_1 getElementFromColumn(rmatrix_t* matrix, const unsigned int element_id) {
    return matrix->data[element_id * (matrix->width) + matrix->column];
}

)");

    // user source
    s.append(_srcUser);

    // allpairs skeleton source
    s.append(
      #include "AllPairsKernel2.cl"
    );

    auto program = detail::Program(s, detail::util::hash("//AllPairs\n"
                                                         + Matrix<Tout>::deviceFunctions()
                                                         + _srcUser
                                                         + _funcUser));
    // modify program
    if (!program.loadBinary()) {
        program.transferParameters(_funcUser, 3, "SCL_ALLPAIRS");
        program.transferArguments(_funcUser, 3, "USR_FUNC");

        program.renameFunction(_funcUser, "USR_FUNC");

        program.adjustTypes<Tleft, Tright, Tout>();
    }

    program.build();

    return program;
}

template<typename Tleft, typename Tright, typename Tout>
void AllPairs<Tout(Tleft, Tright)>::prepareInput(const Matrix<Tleft>& left,
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

template<typename Tleft, typename Tright, typename Tout>
void AllPairs<Tout(Tleft, Tright)>::prepareOutput(Matrix<Tout>& output,
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

#endif // ALLPAIRS_DEF_H
