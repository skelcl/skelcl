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
 
/*
 * SourceLocation.h
 *
 *      Author: Michel Steuwer
 *              Tobias Günnewig
 */

#ifndef SOURCELOCATION_H_
#define SOURCELOCATION_H_

#include <clang-c/Index.h>

namespace ssedit {

class SourceLocation {
public:
  struct Location {
    unsigned int  line;
    unsigned int  column;
    unsigned int  offset;
    CXFile        file;
  };

  SourceLocation();

  SourceLocation(const SourceLocation& rhs);

  SourceLocation(CXSourceLocation location);

  SourceLocation& operator=(const SourceLocation& rhs);

  Location getSpellingLocation() const;

  Location getInstantiationLocation() const;

  CXSourceLocation getCXSourceLocation() const;

private:
  CXSourceLocation _location;
};

} // namespace ssedit

#endif /* SOURCELOCATION_H_ */

