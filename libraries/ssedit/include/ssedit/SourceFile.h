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
 * SourceFile.h
 */

#ifndef SOURCEFILE_H_
#define SOURCEFILE_H_

#include <future>
#include <string>
#include <vector>

#include <clang-c/Index.h>

#include "DeltaTree.h"
#include "CallExpression.h"

namespace ssedit {

class Cursor;
class Entity;
class Function;
class Parameter;
class Typedef;

///
/// \class SourceFile
///
/// \brief This class is the main entry point in the ssedit library and represents
///        a source file to be analyzed or edited.
///
/// This class represents a source file stored on disk to be analyzed or edited.
/// Changes on the source file can be commited and are stored inside the ssedit
/// library and finally applied to the physical source file on disk as the
/// function writeCommittedChanges is called. Keep that in mind as for example
/// renaming of an entity is not immediately reflected after the call of the
/// commitRename function.
///
class SourceFile {
public:
  ///
  /// \brief No default construction allowed
  ///
  SourceFile() = delete;

  ///
  /// \brief Constructs a source file object by interpreting the given string as
  ///        a filename.
  ///
  /// \param fileName A string being interpreted as a filename. The sting is
  ///                 passed unmodified to the constructor of std::ifstream.
  ///
  SourceFile(const std::string& fileName);

  ///
  /// \brief Constructs a source file object by copy from the object rhs
  ///
  /// \param rhs The  object to be copied from
  ///
  SourceFile(const SourceFile& rhs);

  ///
  /// \brief Constructs a source file object by moving from the object rhs
  ///
  /// \param rhs The object to be moved from
  ///
  SourceFile(SourceFile&& rhs);

  ///
  /// \brief Destructs the source file object
  ///
	virtual ~SourceFile();

  ///
  /// \brief Assigns the content of the object rhs to this object
  ///
  /// \param rhs The object from whom assign
  ///
  /// \return A reference to this
  ///
  SourceFile& operator=(const SourceFile& rhs);

  ///
  /// \brief Searches for a function definition based on the name of the
  ///        function
  ///
  /// \param identifier The name of the function to be searched for
  ///
  /// \return If the function isValid called on the returned value returns
  ///         false, then no function with the given identifier was found.
  ///         If the function isValid called on the returned value returns
  ///         true, the returned object contains information about the found
  ///         function definition.
  ///
  Function findFunction(const std::string& identifier);

  ///
  /// \brief Searches for a typedef declaration based on the name of the typedef
  ///
  /// \param identifier The name of the typedef to be searched for
  ///
  /// \return If the function isValid called on the returned value returns
  ///         false, then no typedef with the given identifier was found.
  ///         If the function isValid called on the returned value returns
  ///         true, the returned object contains information about the found
  ///         typedef declaration.
  ///
  Typedef findTypedef(const std::string& identifier);

  ///
  /// \brief Searches for expressions where the function with the name given by
  ///        identifier was called -- named CallExpresions
  ///
  /// \param identifier The name of the function for which CallExpressions
  ///                   should be found
  ///
  /// \return If no function with the identifier exists or no such function is
  ///         ever called an empty vector is returned. Otherwise, an object for
  ///         every found call of the function is returned.
  ///
  std::vector<CallExpression>
    findCallExpressions(const std::string& identifier);

  ///
  /// \brief Initiate the renaming of the given entity to the specified new name
  ///
  /// Note, that this function only initiate the renaming not actually performs
  /// it. The changes are only reflected in the source file after a call of
  /// writeCommittedChanges.
  ///
  /// \param entity   The Entity for which the renaming should be initiated
  ///        newName  The new name for the specified entity
  ///
  void commitRename(Entity& entity, const std::string& newName);

  ///
  /// \brief Initiate the renaming of the given typedef, so that the given name
  ///        is an new alias for the type of the typedef.
  ///
  /// Note, that this function only initiate the type replacement not actually
  /// performs it. The changes are only reflected in the source file after a
  /// call of writeCommittedChanges.
  ///
  /// \param tdef         The Typedef for which the renaming should be initiated
  ///        newTypeName  The new name of the type now an alias of the typedef
  ///
  void commitReplaceType(Typedef& tdef, const std::string& newTypeName);

  ///
  /// \brief Initiate appending the given parameter as parameter to the given
  ///        function definition.
  ///
  /// Note, that this function only initiate the appending of the parameters
  /// not actually performs it. The changes are only reflected in the source
  /// file after a call of writeCommittedChanges.
  ///
  /// \param func   The function on which the parameter should be appended
  ///        param  The parameter which should be appended on the function
  ///
  void commitAppendParameter(Function& func, Parameter& param);

  void commitAppendParameter(Function& func, const std::string& paramAsString);

  ///
  /// \brief Initiate appending the given parameter as argument (just the name
  ///        of the parameter, not the type) to the given callExpression
  ///        (call of a function).
  ///
  /// Note, that this function only initiate the appending of the parameter as
  /// argument not actually performs it. The changes are only reflected in the
  /// source file after a call of writeCommittedChanges.
  ///
  /// \param callExpression   The call expression on which the argument should
  ///                         be appended
  ///        param            The parameter which should be appended as argument
  ///                         (just the name of the parameter, not the type)
  ///                         on the call expression
  ///
  void commitAppendArgument(CallExpression& callExpression,
                            Parameter& param);

  void commitInsertSourceAtFunctionBegin(Function& func,
                                         const std::string& source);

  ///
  /// \brief Write changes, made using calls to the functions prefixed with
  ///        commit, to the source file on disk
  ///
	void writeCommittedChanges();

  ///
  /// \brief Returns the file name of the source file on disk
  ///
  /// \return The file name of the source file on disk as a string
  ///
  std::string getFileName() const;

  ///
  /// \brief Print the changes stored, but not yet applied to the source file
  ///        on standard out. For debug purposes only.
  ///
  void printDeltaTree() {
    _deltaTree.printTree();
  };

private:
  ///
  /// \brief This function lets libclang parse the given filename and returns a
  ///        handle for the parsed data structure.
  ///
  /// \param fileName The name of the source file which should be parsed
  ///
  /// \return A handle to the parsed data structure as returned by libclang
  ///
  CXTranslationUnit parse(const std::string& fileName);

  void writeDelta(const Delta& delta, std::string* source);
  void performReplace(const Delta& delta, std::string* source);
  void performInsert(const Delta& delta, std::string* source);
  void performInsertAfter(const Delta& delta, std::string* source);

  CXIndex                               _index;
  std::shared_future<CXTranslationUnit> _tu;
  CXFile                                _file;
	DeltaTree                             _deltaTree;
};

} // namespace ssedit

#endif /* SOURCEFILE_H_ */

