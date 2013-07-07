#include "stooling/SourceCode.h"

#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Weffc++"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-promo"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wshift-sign-overflow"
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wcast-align"
#pragma GCC diagnostic ignored "-Wduplicate-enum"

#include <clang/AST/Expr.h>
#include <clang/AST/ExprCXX.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Basic/LangOptions.h>
#include <clang/Lex/Lexer.h>

#pragma GCC diagnostic pop

#include <string>

#include "RefactoringTool.h"

#include "TransferArgumentsCallback.h"
#include "TransferParametersCallback.h"
#include "RenameFunctionCallback.h"
#include "RenameTypedefCallback.h"
#include "RedefineTypedefCallback.h"
#include "FixKernelParameterCallback.h"

#include <iostream>

using namespace clang;
using namespace clang::ast_matchers;
using clang::tooling::newFrontendActionFactory;

namespace stooling {

SourceCode::SourceCode(const std::string& source)
  : _source(source), _tool(new RefactoringTool())
{
}

SourceCode::SourceCode(SourceCode&& rhs)
  : _source(std::move(rhs._source)), _tool(rhs._tool)
{
  rhs._tool = nullptr;
}

SourceCode& SourceCode::operator=(SourceCode&& rhs)
{
  _source = std::move(rhs._source);
  _tool   = rhs._tool; rhs._tool = nullptr;
  return *this;
}

SourceCode::~SourceCode()
{
  delete _tool;
}

void SourceCode::transferParameters(const std::string& from,
                                unsigned int startIndex,
                                const std::string& to)
{
  // First extract the parameters from the "from" function into "parameter"
  std::string parameter;
  ast_matchers::MatchFinder parameterFinder;
  ExtractParametersCallback extractCallback(&parameter, startIndex);
  parameterFinder.addMatcher(
      functionDecl(hasName(from)).bind("fromDecl"),
      &extractCallback);

  _tool->run(_source, newFrontendActionFactory(&parameterFinder));

  if (!parameter.empty()) {
    // Then insert the parameters to the declaration of the "to" function
    ast_matchers::MatchFinder functionFinder;
    ApplyParametersCallback applyCallback(_tool->replacements(), parameter);
    functionFinder.addMatcher(
        functionDecl(hasName(to)).bind("toDecl"),
        &applyCallback);

    _source = _tool->transform(_source,
                               newFrontendActionFactory(&functionFinder));
  }
}

void SourceCode::transferArguments(const std::string& from,
                               unsigned int startIndex,
                               const std::string& to)
{
  // First extract argument names from the "from" function declaration into
  // "arguments"
  std::string arguments;
  ast_matchers::MatchFinder argumentsFinder;
  ExtractArgumentsCallback extractCallback(&arguments, startIndex);
  argumentsFinder.addMatcher(
      functionDecl(hasName(from)).bind("fromDecl"),
      &extractCallback);
  _tool->run(_source, newFrontendActionFactory(&argumentsFinder));
  
  if (!arguments.empty()) {
    // Then insert the arguments to every call of the function "to"
    ast_matchers::MatchFinder callFinder;
    ApplyArgumentsCallback applyCallback(_tool->replacements(), arguments);
    callFinder.addMatcher(
        callExpr(callee(functionDecl(hasName(to)))).bind("toCall"),
        &applyCallback);

    _source = _tool->transform(_source, newFrontendActionFactory(&callFinder));
  }
}

void SourceCode::renameFunction(const std::string& from, const std::string& to)
{
  ast_matchers::MatchFinder finder;
  RenameFunctionCallback callback(_tool->replacements(), to);
  // match function declarations with the name "from"
  finder.addMatcher(
      functionDecl(hasName(from)).bind("decl"),
      &callback);
  // match call expressions calling a function with the name "from"
  finder.addMatcher(
      callExpr(callee(functionDecl(hasName(from)))).bind("call"),
      &callback);

  _source = _tool->transform(_source, newFrontendActionFactory(&finder));
}

void SourceCode::renameTypedef(const std::string& from, const std::string& to)
{
  ast_matchers::MatchFinder finder;
  RenameTypedefCallback callback(_tool->replacements(), from, to);
  // match any named declaration
  // filter further in the callback
  finder.addMatcher(
      namedDecl().bind("decl"),
      &callback);

  _source = _tool->transform(_source, newFrontendActionFactory(&finder));
}

void SourceCode::redefineTypedef(const std::string& typedefName,
                                 const std::string& newType)
{
  ast_matchers::MatchFinder finder;
  RedefineTypedefCallback callback(_tool->replacements(), typedefName, newType);
  // match any named declaration
  // filter further in the callback
  finder.addMatcher(
      namedDecl().bind("decl"),
      &callback);

  _source = _tool->transform(_source, newFrontendActionFactory(&finder));
}

void SourceCode::fixKernelParameter(const std::string& kernel)
{
  ast_matchers::MatchFinder finder;
  FixKernelParameterCallback callback(_tool->replacements());
  // match function declarations with the name "from"
  finder.addMatcher(
      functionDecl(hasName(kernel)).bind("decl"),
      &callback);

  _source = _tool->transform(_source, newFrontendActionFactory(&finder));
}

const std::string& SourceCode::code()
{
  return _source;
}

} // namespace stooling

