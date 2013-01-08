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
/// \file AllPairs.h
///
///	\author Malte Friese <malte.friese@uni-muenster.de>
///

#ifndef ALLPAIRS_ZR_H
#define ALLPAIRS_ZR_H

#include <istream>
#include <string>

#include "detail/Skeleton.h"

namespace skelcl {

template <typename> class Matrix;
template <typename> class Reduce;
template <typename> class Zip;
template <typename> class Out;
namespace detail { class Program; }

template<typename> class AllPairsZR;


template<typename Tleft,
         typename Tright,
         typename Tout>
class AllPairsZR<Tout(Tleft, Tright)> : public AllPairsBase<Tout(Tleft, Tright)> {

    public:
    // Konstruktor
    AllPairsZR<Tout(Tleft, Tright)>(const Reduce<Tout(Tout)>& reduce, const Zip<Tout(Tleft, Tright)>& zip);

    protected:
    // Programm erstellen
    detail::Program createAndBuildProgram() const;

    std::string _srcReduce;
    std::string _srcZip;
    std::string _funcReduce;
    std::string _funcZip;
    std::string _idReduce;
};

} // namespace skelcl

#include "detail/AllPairsZRDef.h"

#endif // ALLPAIRS_ZR_H
