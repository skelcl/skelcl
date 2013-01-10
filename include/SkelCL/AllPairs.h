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

#ifndef ALLPAIRS_H
#define ALLPAIRS_H

//#include <istream>
#include <string>

#include "AllPairsBase.h"
#include "AllPairsStr.h"
#include "AllPairsZR.h"
#include "Matrix.h"
#include "Reduce.h"
#include "Zip.h"
#include "Out.h"


namespace skelcl {

template<typename> class AllPairs;

template<typename Tleft,
         typename Tright,
         typename Tout>
class AllPairs<Tout(Tleft, Tright)> {

    public:
    // Konstruktor
    AllPairs<Tout(Tleft, Tright)>(const Reduce<Tout(Tout)>& reduce, const Zip<Tout(Tleft, Tright)>& zip);
    AllPairs<Tout(Tleft, Tright)>(const std::string& source, const std::string& func = std::string("func"));

    // Destruktor
    ~AllPairs();

    AllPairs(const AllPairs& rhs) = default; // geht das?
    AllPairs& operator=(const AllPairs& rhs) = default;

    // Ausführungsoperator
    template <typename... Args>
    Matrix<Tout> operator()(const Matrix<Tleft>& left,
                            const Matrix<Tright>& right,
                            Args&&... args);

    // Ausführungsoperator mit Referenz
    template <typename... Args>
    Matrix<Tout>& operator()(Out< Matrix<Tout> > output,
                             const Matrix<Tleft>& left,
                             const Matrix<Tright>& right,
                             Args&&... args);

    private:
    AllPairsBase<Tout(Tleft, Tright)> *allpairs;
};

// Konstruktor 1
template<typename Tleft, typename Tright, typename Tout>
AllPairs<Tout(Tleft, Tright)>::AllPairs(const Reduce<Tout(Tout)>& reduce, const Zip<Tout(Tleft, Tright)>& zip)
{
    allpairs = new AllPairsZR<Tout(Tleft, Tright)>(reduce, zip);
}

// Konstruktor 2
template<typename Tleft, typename Tright, typename Tout>
AllPairs<Tout(Tleft, Tright)>::AllPairs(const std::string& source, const std::string& func)
{
    allpairs = new AllPairsStr<Tout(Tleft, Tright)>(source, func);
}

// Destruktor
template<typename Tleft, typename Tright, typename Tout>
AllPairs<Tout(Tleft, Tright)>::~AllPairs() {
    delete allpairs;
    allpairs = 0;
}


// public interface
template<typename Tleft, typename Tright, typename Tout>
template <typename... Args>
Matrix<Tout> AllPairs<Tout(Tleft, Tright)>::operator()(const Matrix<Tleft>& left,
                                                       const Matrix<Tright>& right,
                                                       Args&&... args)
{
    return allpairs->operator()(left, right, std::forward<Args>(args)...);
}

template<typename Tleft, typename Tright, typename Tout>
template <typename... Args>
Matrix<Tout>& AllPairs<Tout(Tleft, Tright)>::operator()(Out< Matrix<Tout> > output,
                                                        const Matrix<Tleft>& left,
                                                        const Matrix<Tright>& right,
                                                        Args&&... args)
{
    return allpairs->operator()(output, left, right, std::forward<Args>(args)...);
}

} // namespace skelcl

#endif // ALLPAIRS_H
