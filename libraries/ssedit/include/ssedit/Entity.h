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
 * Entity.h
 */

#include <string>

#include "Cursor.h"

#ifndef ENTITY_H_
#define ENTITY_H_

namespace ssedit {

class SourceFile;

///
/// \class Entity
///
/// \brief An instance of the class represents an entity in the source file,
///        like a variable declaration or a function definition.
///
/// This is a base class, more specific classes represent different types of
/// entities, like Function or Parameter.
///
class Entity {
public:
  ///
  /// \brief No default construction
  ///
  Entity() = delete;

  ///
  /// \brief Constructs an entity from a cursor and the corresponding source
  ///        file
  ///
  /// \param cursor     Cursor pointing at the entity in the source file
  ///        sourceFile Source file the entity is defined in
  ///
  Entity(const Cursor& cursor, SourceFile& sourceFile);

  ///
  /// \brief Copy constructs an entity from rhs
  ///
  /// \param rhs The object from which the content is copied to this
  ///
  Entity(const Entity& rhs);

  ///
  /// \brief Assigns the content of rhs to this
  ///
  /// \param rhs The object from which the content is assigned to this
  ///
  /// \return A reference to *this
  ///
  Entity& operator=(const Entity& rhs);

  ///
  /// \brief Default destructor
  ///
  virtual ~Entity();

  ///
  /// \brief Returns the name of the entity as a string
  ///
  /// \return The name of the Entity as a string
  ///
  std::string getName() const;

  ///
  /// \brief Returns the cursor for the entity. See Cursor for details.
  ///
  /// \return The cursor for the current entity.
  ///
  Cursor getCursor() const;

  ///
  /// \brief Returns true if the entity represents something meaningful, like a
  ///        function declaration, etc. Returns false if the entity cannot be
  ///        used meaningful.
  ///
  /// \return True if the entity represents something meaningful, like a
  ///         function declaration, etc. Returns false if the entity cannot be
  ///         used meaningful.
  ///
  bool isValid() const;

protected:
  Cursor      _cursor;
  SourceFile* _sourceFile;
};

} // namespace ssedit

#endif /* ENTITY_H_ */

