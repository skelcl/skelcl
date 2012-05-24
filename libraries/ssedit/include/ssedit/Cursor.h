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
/// Cursor.h
///

#ifndef CURSOR_H_
#define CURSOR_H_

#include <clang-c/Index.h>

#include <string>
#include <vector>
#include <functional>

#include "SourceLocation.h"
#include "SourceRange.h"
#include "Type.h"

namespace ssedit {

///
/// \class Cursor
///
/// \brief A cursor points to some entity in the source file.
///
/// A cursor points to some entity in the source file. Cursors are organized
/// hierarchical, i.e., a cursor might have other cursor as children.
/// For example, a cursor pointing to a function declaration has the cursors
/// pointing to the parameters of the functions as its children.
/// This class can be used to get information for a given cursor, or search the
/// children of a given cursor for a special cursor.
/// Every cursor has a kind, which indicated to what kind of entity a cursor is
/// pointing.
///
class Cursor {
public:
  ///
  /// \brief Constructs an invalid cursor
  ///
  Cursor();

  ///
  /// \brief Constructs a cursor pointing to the hole translation unit
  ///        (the source file)
  ///
  /// Such a constructed cursor can be used to search for entities in the hole
  /// source file.
  ///
  /// \param tu The libclang translation unit, representing a parsed source file
  ///
  Cursor(CXTranslationUnit tu);

  ///
  /// \brief Construct a cursor which by providing the low-level libclang
  ///        equivalent, a CXCursor
  ///
  /// \param cursor The libclang cursor from which the cursor is created
  ///
  Cursor(CXCursor cursor);

  ///
  /// \brief Construct a cursor by copy from rhs
  ///
  /// \param rhs The cursor from which a copy is created
  ///
  Cursor(const Cursor &rhs);

  ///
  /// \brief Assignment operator
  ///
  /// \param rhs The cursor which is used to assign to this
  ///
  /// \return A reference to *this
  ///
  Cursor& operator=(const Cursor &rhs);

  ///
  /// \brief Returns the Type of the cursor
  ///
  /// \return The Type of the cursor
  ///
  Type getType() const;

  ///
  /// \brief Returns the result type associated with a function type, if cursor
  ///        is pointing to a function. Otherwise an invalid type is returned.
  ///
  /// \return If the current cursor points to a function, the result type of the
  ///         function is returned. Otherwise an invalid type is returned.
  ///
  Type getResultType() const;

  ///
  /// \brief If the cursor points to a reference, the cursor pointing to the
  ///        original is returned. Otherwise an invalid cursor is returned.
  ///
  /// \return If the cursor points to a reference, the cursor pointing to the
  ///        original is returned. Otherwise an invalid cursor is returned.
  ///
  Cursor getReferenced() const;

  ///
  /// \brief Checks if the given cursor is referenced by the current cursor
  ///
  /// \param referenced The cursor for which is check, if the current cursor
  ///                   references it
  ///
  /// \return Returns true iff the given cursor is referenced by the current
  ///         cursor. I.e. this->getReferenced() == referenced
  ///
  bool isReferenced(const Cursor& referenced) const;

  ///
  /// \brief Returns the physical extent of the entity the current cursor is
  ///        pointing to in the source file.
  ///
  /// \return Returns a range in the source file describing the physical extent
  ///         of the entity pointed to by the current cursor.
  ///         If the current cursor is invalid an invalid range is returned.
  ///
  SourceRange getExtent() const;

  ///
  /// \brief Returns the physical location of the entity the current cursor is
  ///        pointing to in the source file.
  ///
  /// \return Returns a location in the source file describing the physical
  ///         location of the entity pointed to by the current cursor.
  ///         If the current cursor is invalid an invalid location is returned.
  ///
  SourceLocation getLocation() const;

  ///
  /// \brief Searches all children of the current cursor for a cursor pointing
  ///        to a function and for which the spelling is equal to the given
  ///        identifier.
  ///        The first matching cursor is returned, or an invalid cursor if no
  ///        such child exist.
  ///
  /// \param identifier The identifier which is compared to the spelling of all
  ///                   children of the current cursor, which are pointing to
  ///                   function definitions.
  ///
  /// \return Returns the first children of the current cursor pointing to a
  ///         function definition and for which the spelling is equal to the
  ///         given identifier. If no such child exists an invalid cursor is
  ///         returned.
  ///
  Cursor findFunctionCursor(const std::string& identifier) const;

  ///
  /// \brief Searches all children of the current cursor for a cursor pointing
  ///        to a typedef and for which with the spelling is equal to the given
  ///        identifier.
  ///        The first matching cursor is returned, or an invalid cursor if no
  ///        such child exist.
  ///
  /// \param identifier The identifier which is compared to the spelling of all
  ///                   children of the current cursor, which are pointing to
  ///                   typedefs.
  ///
  /// \return Returns the first children of the current cursor pointing to a
  ///         typedef and for which the spelling is equal to the given
  ///         identifier. If no such child exists an invalid cursor is returned.
  ///
  Cursor findTypedefCursor(const std::string& identifier) const;

  ///
  /// \brief Searches all children of the current cursor for a cursor pointing
  ///        to a call expression and for which the spelling is equal to the
  ///        given identifier.
  ///        All matching cursors are added to the given expressions vector.
  ///
  /// \param identifier  The identifier which is compared to the spelling of all
  ///                    children of the current cursor, which are pointing to
  ///                    call expression.
  ///        expressions This vector of cursors is filled with all children
  ///                    pointing to a call expression and for which the
  ///                    spelling is equal to the given identifier.
  ///
  void findCallExpressionCursors(const std::string& identifier,
                                 std::vector<Cursor>* expressions) const;

  ///
  /// \brief Calls the given filter function on all immediate children of the
  ///        given cursor. All children for which the filter returns true are
  ///        added to the given vector.
  ///
  /// \param vector This vector of cursors is filled with all immediate children
  ///               for which the given filter function returns true.
  ///        filter This function is applied to every immediate children and can
  ///               decide whether to add the child to the vector or not.
  ///
  void gatherImmediateChildren(std::vector<Cursor>* vector,
                               const std::function<bool (const Cursor&)>& filter) const;

  ///
  /// \brief Calls the given filter function on all children of the
  ///        given cursor. All children for which the filter returns true are
  ///        added to the given vector.
  ///
  /// \param vector This vector of cursors is filled with all children
  ///               for which the given filter function returns true.
  ///        filter This function is applied to every immediate children and can
  ///               decide whether to add the child to the vector or not.
  ///
  void gatherChildren(std::vector<Cursor>* vector,
                      const std::function<bool (const Cursor&)>& filter) const;

  ///
  /// \brief Returns the name of the entity the current cursor is pointing to
  ///
  /// \return The name of the entity the current pointer is pointing to as a
  ///         string
  ///
  std::string getSpelling() const;

  ///
  /// \brief Compares the given string with the spelling of the current cursor
  ///
  /// \return Returns true iff the given string is equal to the spelling of the
  ///         current cursor
  ///
  bool spellingIs(const std::string& spelling) const;

  ///
  /// \brief Gets access to the low-level libclang cursor equivalent
  ///
  /// \return The corresponding libclang cursor equivalent
  ///
  CXCursor getCXCursor() const;

  ///
  /// \brief Returns the kind of entity the cursor is pointing to, e.g. a
  ///        function definition, a variable declaration, ...
  ///
  /// See libclang manual for possible values of CXCursorKind
  ///
  /// \return The kind of the current cursor
  ///
  CXCursorKind getKind() const;

  ///
  /// \brief Checks if the current cursor is of the given kind
  ///
  /// \return Returns true iff the given cursor kind is equal to the kind of
  ///         the current cursor
  ///
  bool isOfKind(CXCursorKind) const;

  ///
  /// \brief Checks if the current cursor is a null cursor, i.e. pointing to no
  ///        meaningful entity.
  ///
  /// \return Returns true iff the current cursor is a null cursor
  ///
  bool isNullCursor() const;

  ///
  /// \brief Checks if the current cursor is valid, i.e. pointing to something
  ///        this library can handle.
  /// Note, that isNullCursor == false does not imply isValid == true.
  /// On the other hand, isNullCursor == true implies isValid == false.
  ///
  /// \return Returns true iff the current cursor is valid.
  ///
  bool isValid() const;

private:
  CXCursor _cursor;
};

} // namespace ssedit

#endif /* CURSOR_H_ */

