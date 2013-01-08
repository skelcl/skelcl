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
/// \file AllPairsZRDef.h
///
///	\author Malte Friese <malte.friese@uni-muenster.de>
///

#ifndef ALLPAIRS_ZR_DEF_H
#define ALLPAIRS_ZR_DEF_H

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
AllPairsZR<Tout(Tleft, Tright)>::AllPairsZR(const Reduce<Tout(Tout)>& reduce, const Zip<Tout(Tleft, Tright)>& zip)
    : _srcReduce(reduce.source()),
      _srcZip(zip.source()),
      _funcReduce(reduce.func()),
      _funcZip(zip.func()),
      _idReduce(reduce.id())
{
    this->_C = 32;
    this->_R = 8;
    this->_S = 16;

    LOG_DEBUG("Create new AllPairs object (", this, ")");
}

// Programm erstellen
template<typename Tleft, typename Tright, typename Tout>
detail::Program AllPairsZR<Tout(Tleft, Tright)>::createAndBuildProgram() const
{
    ASSERT_MESSAGE( !_srcReduce.empty(),
                    "Tried to create program with empty user reduce source." );
    ASSERT_MESSAGE( !_srcZip.empty(),
                    "Tried to create program with empty user zip source." );

    // _srcReduce: replace func by TMP_REDUCE
    ssedit::TempSourceFile reduceTemp(_srcReduce);

    auto func = reduceTemp.findFunction(_funcReduce); ASSERT(func.isValid());
    reduceTemp.commitRename(func, "TMP_REDUCE");
    reduceTemp.writeCommittedChanges();

    std::ifstream rFile(reduceTemp.getFileName());
    std::string rSource( (std::istreambuf_iterator<char>(rFile)),
                         std::istreambuf_iterator<char>() );

    // _srcZip: replace func by TMP_ZIP
    ssedit::TempSourceFile zipTemp(_srcZip);

    func = zipTemp.findFunction(_funcReduce); ASSERT(func.isValid());
    zipTemp.commitRename(func, "TMP_ZIP");
    zipTemp.writeCommittedChanges();

    std::ifstream zFile(zipTemp.getFileName());
    std::string zSource( (std::istreambuf_iterator<char>(zFile)),
                         std::istreambuf_iterator<char>() );

    // create program
    std::string s(Matrix<Tout>::deviceFunctions());

    // identity
    s.append("#define SCL_IDENTITY ").append(_idReduce);

    s.append("\n");

    // reduce user source
    s.append(rSource);

    s.append("\n");

    // zip user source
    s.append(zSource);

    // allpairs skeleton source
    s.append(
      #include "AllPairsKernel.cl"
    );

    auto program = detail::Program(s, detail::util::hash("//AllPairs\n"
                                                         + Matrix<Tout>::deviceFunctions()
                                                         + _idReduce
                                                         + rSource
                                                         + zSource));
    // modify program
    if (!program.loadBinary()) {
        program.transferParameters("TMP_REDUCE", 2, "SCL_ALLPAIRS"); // problem: reduce parameter a und zip parameter a
        program.transferArguments("TMP_REDUCE", 2, "USR_REDUCE");
        program.transferParameters("TMP_ZIP", 2, "SCL_ALLPAIRS"); // reihenfolge? erst args von reduce dann zip?
        program.transferArguments("TMP_ZIP", 2, "USR_ZIP");

        program.renameFunction("TMP_REDUCE", "USR_REDUCE");
        program.renameFunction("TMP_ZIP", "USR_ZIP");

        program.adjustTypes<Tleft, Tright, Tout>();
    }

    program.build();

    return program;
}

} // namespace skelcl

#endif // ALLPAIRS_ZR_DEF_H
