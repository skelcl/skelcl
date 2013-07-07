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
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wcast-align"
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wshift-sign-overflow"
#pragma GCC diagnostic ignored "-Wduplicate-enum"
#endif

#include <clang/AST/Expr.h>
#include <clang/AST/ExprCXX.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Basic/LangOptions.h>
#include <clang/Lex/Lexer.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/Refactoring.h>

#pragma GCC diagnostic pop

#include <string>
#include <sstream>
#include <iostream>

#include "TransferArgumentsCallback.h"
#include "Utilities.h"

using namespace clang;
using namespace clang::tooling;

namespace stooling {

ExtractArgumentsCallback::ExtractArgumentsCallback(
    std::string* arguments,
    unsigned int startIndex)
  : _arguments(*arguments), _startIndex(startIndex)
{}

void ExtractArgumentsCallback::run(
    const ast_matchers::MatchFinder::MatchResult& result)
{
  const FunctionDecl* funcDecl
    = result.Nodes.getDeclAs<FunctionDecl>("fromDecl");
  if (funcDecl) {
    std::ostringstream oss;
    for ( clang::FunctionDecl::param_const_iterator
            i  = funcDecl->param_begin() + _startIndex,
            e  = funcDecl->param_end();
            i != e;
          ++i ) {
      oss << (*i)->getNameAsString();
      if (i + 1 != e) { oss << ", "; }
    }
    _arguments = oss.str();
  }
}

ApplyArgumentsCallback::ApplyArgumentsCallback(
                          clang::tooling::Replacements& replacements,
                          const std::string& arguments)
  : _replacements(replacements), _arguments(arguments)
{}

void ApplyArgumentsCallback::run(
    const ast_matchers::MatchFinder::MatchResult& result)
{
  const CallExpr* callExpr = result.Nodes.getDeclAs<CallExpr>("toCall");
  if (callExpr) {
    if (callExpr->getNumArgs() > 0) {
      const Expr* lastArgExpr = callExpr->getArg(callExpr->getNumArgs() - 1);
      std::string lastArg = getText(*result.SourceManager,
                                    *lastArgExpr);

      _replacements.insert(Replacement(*result.SourceManager,
                                       lastArgExpr,
                                       lastArg + ", " + _arguments));
    } else {
      // look for the next token: a '('
      clang::SourceLocation insertLoc =
        clang::Lexer::findLocationAfterToken(callExpr->getLocStart(),
                                             clang::tok::l_paren,
                                             *result.SourceManager,
                                             clang::LangOptions(),
                       /*skip Whitespace? */ true);

      _replacements.insert(Replacement(*result.SourceManager,
                                       insertLoc, 0,
                                       _arguments));
    }
  }
}

} // namespace stooling

