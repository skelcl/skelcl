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
/// \file Stencil.h
///
///	\author Stefan Breuer<s_breu03@uni-muenster.de>
///
#ifndef STENCIL_H_
#define STENCIL_H_

#include <istream>
#include <string>
#include <vector>

#include "detail/Padding.h"
#include "detail/Skeleton.h"
#include "detail/Program.h"

namespace skelcl {

template<typename > class Matrix;
template<typename > class Out;
template<typename > class StencilInfo;

template<typename > class Stencil;

template<typename Tin,
         typename Tout>
class Stencil<Tout(Tin)> : public detail::Skeleton {

public:
    Stencil<Tout(Tin)>(const Source& source, unsigned int north, unsigned int west, unsigned int south, unsigned int east,
                       detail::Padding padding, Tin neutral_element, const std::string& func);

    Stencil<Tout(Tin)>(const Source& source, unsigned int west, unsigned int east,
                                detail::Padding padding, Tin neutral_element, const std::string& func);

    // Ausführungsoperator
    template<typename ... Args>
    Matrix<Tout> operator()(unsigned int iterations, const Matrix<Tin>& in, Args&&... args);

    // Ausführungsoperator mit Referenz
    template<typename ... Args>
    Matrix<Tout>& operator()(unsigned int iterations, Out<Matrix<Tout> > output,
//                             Out<Matrix<Tout>> tmp,
                             const Matrix<Tin>& in,
            Args&&... args);
    void add(const Source& source, unsigned int north, unsigned int west, unsigned int south, unsigned int east,
             detail::Padding padding, Tin neutral_element, const std::string& func);
private:

	// Ausführen
    template<typename ... Args>
    void execute(Matrix<Tout>& output,
//                 Matrix<Tout>& tmp,
                 const Matrix<Tin>& in, Args&&... args);

    unsigned int determineLargestNorth();
    unsigned int determineLargestWest();
    unsigned int determineLargestSouth();
    unsigned int determineLargestEast();

    // Eingabe vorbereiten
    void prepareInput(const Matrix<Tin>& in);

    // Ausgabe vorbereiten
    void prepareOutput(Matrix<Tout>& output, const Matrix<Tin>& in);

    std::vector<StencilInfo<Tout(Tin)> > _stencilInfos;

    unsigned int _iterations;
};

} //namespace skelcl

#include "detail/StencilDef.h"

#endif /* STENCIL_H_ */