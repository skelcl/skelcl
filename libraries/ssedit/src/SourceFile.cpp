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
 * SourceFile.cpp
 */

#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <future>
#include <iterator>
#include <memory>

#include <clang-c/Index.h>

#include <pvsutil/Logger.h>

#include "ssedit/SourceFile.h"
#include "ssedit/DeltaTree.h"
#include "ssedit/Function.h"
#include "ssedit/Entity.h"
#include "ssedit/Typedef.h"
#include "ssedit/CallExpression.h"
#include "ssedit/Cursor.h"

namespace { // unnamed namespace

using namespace ssedit;

struct IsReferenceOfCursor {
  IsReferenceOfCursor(const Cursor& cursor) : _cursor(cursor) {}
  bool operator()(const Cursor& current) {
    return current.isReferenced(_cursor);
  }
  Cursor _cursor;
};

// adds __global modifier, if no global modifier is found
// if a local modifier is found, it is replaced
// after this function call the paramToken definetly
// contains a global modifer
void addGlobalAddressSpaceModifier(std::string* paramToken)
{
  // __global, global, __local and local are OpenCL keywords and can,
  // therefore, not be used as variable names => simple searching for them
  // should be sufficient enough (ignoring e.g. comments)

  if (   paramToken->find("__global") == std::string::npos
      && paramToken->find(  "global") == std::string::npos ) {
    // if global modifier cannot be found look for local modifier
    if (   paramToken->find("__local") == std::string::npos
        && paramToken->find(  "local") == std::string::npos ) {
      // local modifier also not found => assume global modifier
      paramToken->insert(0, "__global ");
    }
  }
}

} // unnamed namespace

namespace ssedit {

SourceFile::SourceFile(const std::string& fileName)
  : _index(), _tu(), _file(), _deltaTree()
{
  { // test if file with fileName can be opened
    std::ifstream file(fileName);
    if (file.fail()) {
      std::string msg("Error opening file with filename: ");
      msg += fileName;
      throw std::ios_base::failure(msg.c_str());
    }
  }

  _index = clang_createIndex(0,0);
  // async call to parse translation unit
  _tu = std::async([=]() {
        return parse(fileName);
      });
}


SourceFile::SourceFile(const SourceFile& rhs)
  : _index(rhs._index), _tu(rhs._tu),
    _file(rhs._file), _deltaTree(rhs._deltaTree)
{
}

SourceFile::SourceFile(SourceFile&& rhs)
  : _index(rhs._index), _tu(std::move(rhs._tu)),
    _file(rhs._file), _deltaTree(rhs._deltaTree)
{
}


SourceFile::~SourceFile()
{
  clang_disposeTranslationUnit(_tu.get());
  clang_disposeIndex(_index);
}


SourceFile& SourceFile::operator=(const SourceFile& rhs)
{
  if (this == &rhs) return *this; // handle self assignment
  _index      = rhs._index;
  _tu         = rhs._tu;
  _file       = rhs._file;
  _deltaTree  = rhs._deltaTree;
  return *this;
}


CXTranslationUnit SourceFile::parse(const std::string& fileName)
{
  CXTranslationUnit tu;
#if 1
  const char* command_line_args[] = {"-x", "cl"};
  tu = clang_parseTranslationUnit(_index, fileName.c_str(),
                                  command_line_args, 2, NULL, 0,
                                  CXTranslationUnit_None);
#else
  tu = clang_parseTranslationUnit(_index, fileName.c_str(),
                                  NULL, 0, NULL, 0,
                                  CXTranslationUnit_None);
#endif
  _file = clang_getFile(tu, fileName.c_str());
  return tu;
}


Function SourceFile::findFunction(const std::string& identifier)
{
  return Function(Cursor(_tu.get()).findFunctionCursor(identifier), *this);
}


Typedef SourceFile::findTypedef(const std::string& identifier)
{
  return Typedef(Cursor(_tu.get()).findTypedefCursor(identifier), *this);
}

std::vector<CallExpression>
  SourceFile::findCallExpressions(const std::string& identifier)
{
  std::vector<Cursor> callExpressionCursors;

  Cursor(_tu.get()).findCallExpressionCursors(identifier,
                                              &callExpressionCursors);
  std::vector<CallExpression> callExpressions;
  callExpressions.reserve( callExpressionCursors.size() );

  std::for_each( callExpressionCursors.begin(),
                 callExpressionCursors.end(),
                 [this, &callExpressions](const Cursor& cursor){
                  callExpressions.push_back( CallExpression(cursor, *this) );
                 });
  return callExpressions;
}


void SourceFile::commitRename(Entity& entity, const std::string& newName)
{
  if (!entity.isValid()) {
    return;
  }

  std::vector<Cursor> references;
  Cursor(_tu.get()).gatherChildren(&references,
                                   ::IsReferenceOfCursor(entity.getCursor())
                                  );

  for (auto iter  = references.begin();
            iter != references.end();
          ++iter) {
    SourceLocation start = iter->getLocation();

    auto offset = start.getSpellingLocation().offset;
    auto size   = static_cast<unsigned int>(iter->getSpelling().size());

    SourceLocation end( clang_getLocationForOffset( _tu.get(), _file,
                            offset + size )
                      );
    _deltaTree.insertDelta( Delta(SourceRange(start, end),
                            newName,
                            Delta::REPLACE) );
  }
}


void SourceFile::commitReplaceType(Typedef& tdef,
                                   const std::string& newTypeName)
{
  if (!tdef.isValid()) {
    return;
  }

  std::stringstream sstr;
  sstr << "typedef " << newTypeName << " " << tdef.getName();

  _deltaTree.insertDelta( Delta(tdef.getCursor().getExtent(),
                          sstr.str(),
                          Delta::REPLACE) );
}


void SourceFile::commitAppendParameter(Function& func,
                                       const std::string& paramAsString)
{
  if (!func.isValid()) {
    return;
  }

  auto lastParam = func.getParameters().back();

  SourceLocation lastParamLoc = lastParam.getCursor().getLocation();

  auto offset = lastParamLoc.getSpellingLocation().offset;
  auto size   = static_cast<unsigned int>(lastParam.getCursor().getSpelling().size());

  SourceLocation insertLoc    = clang_getLocationForOffset( _tu.get(), _file,
                                  offset + size );



  _deltaTree.insertDelta( Delta(SourceRange(insertLoc, insertLoc),
                                "," + paramAsString,
                                Delta::INSERT) );
}


void SourceFile::commitAppendParameter(Function& func, Parameter& param)
{
  if (!func.isValid()) {
    return;
  }

  SourceRange paramRange = param.getCursor().getExtent();

  // read new token from file
  std::string paramToken;
  {
    std::ifstream file(getFileName());

    std::unique_ptr<char[]> buffer(new char[paramRange.length()]);

    file.seekg( paramRange.getStart().offset );
    file.read( buffer.get(), paramRange.length() );

    paramToken.append( buffer.get(), paramRange.length() );
  }

  if (param.getType().isPointer()) {
    // add global address space modifier (if necessary)
    ::addGlobalAddressSpaceModifier(&paramToken);
  }

  commitAppendParameter(func, paramToken);
}


void SourceFile::commitAppendArgument(CallExpression& callExpression,
                                      Parameter& param)
{
  if (!callExpression.isValid()) {
    return;
  }

  SourceRange range = callExpression.getCursor().getExtent();

  SourceLocation insertLoc( clang_getLocationForOffset(
                                          _tu.get(), _file,
                                          range.getEnd().offset-1
                                                      )
                          );

  std::string paramToken(",");
  paramToken += param.getName();

  _deltaTree.insertDelta( Delta(SourceRange(insertLoc, insertLoc),
                                paramToken,
                                Delta::INSERT) );
}


void SourceFile::commitInsertSourceAtFunctionBegin(Function& func,
                                                   const std::string& source)
{
  if (!func.isValid()) {
    return;
  }

  auto functionBody = func.getFunctionBody();

  SourceRange range = functionBody.getExtent();
  SourceLocation insertLoc( clang_getLocationForOffset(
                                          _tu.get(), _file,
                                          range.getStart().offset+1
                                                      )
                          );
  _deltaTree.insertDelta( Delta(SourceRange(insertLoc, insertLoc),
                                source,
                                Delta::INSERT) );
}


void SourceFile::writeCommittedChanges()
{
  if (_deltaTree.empty()) {
    return; // nothing to do
  }
  std::string source;
  {
    // open file
    std::ifstream file(getFileName());
    // read content in string
    source.assign( (std::istreambuf_iterator<char>(file)),
                   (std::istreambuf_iterator<char>())      );
  }

  for (auto iter  = _deltaTree.rbegin();
            iter != _deltaTree.rend();
          ++iter) {
    writeDelta(*iter, &source);
  }

  {// TODO: remove if libclang is able to parse OpenCL C code
    size_t found = source.find("/*!!*/");
    if (found != std::string::npos) {
      source.erase(0, found);
    }
  }
  {
    std::ofstream file(getFileName(), std::fstream::trunc);
    // write changed content back to file
    file.write(source.c_str(), static_cast<long>(source.size()));
  }
}


void SourceFile::writeDelta(const Delta& delta, std::string* source)
{
  switch (delta.getKind()) {
    case Delta::REPLACE :
      performReplace(delta, source);
      break;
    case Delta::INSERT :
      if (delta.getNextDelta() != nullptr) {
        writeDelta(*delta.getNextDelta(), source);
      }
      performInsert(delta, source);
      break;
  }
}


void SourceFile::performReplace(const Delta& delta, std::string* source)
{
  // ASSERT(delta.getKind() == Delta::REPLACE);
  auto offset = delta.getRange().getStart().offset;
  source->replace( offset,
                   delta.getRange().length(),
                   delta.getNewToken() );
}


void SourceFile::performInsert(const Delta& delta, std::string* source)
{
  // ASSERT(delta.getKind() == Delta::INSERT || ...);
  auto offset = delta.getRange().getStart().offset;
  source->insert( offset, delta.getNewToken() );
}


std::string SourceFile::getFileName() const
{
  _tu.wait(); // wait until _file is available
  CXString cxFileName = clang_getFileName(_file);
  std::string fileName(clang_getCString(cxFileName));
  clang_disposeString(cxFileName);
  return fileName;
}

} // namespace ssedit

