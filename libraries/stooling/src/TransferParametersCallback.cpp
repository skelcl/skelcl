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
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
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

#include "TransferParametersCallback.h"
#include "Utilities.h"

using namespace clang;
using namespace clang::tooling;

namespace stooling {

ExtractParametersCallback::ExtractParametersCallback(
    std::string* parameter,
    unsigned int startIndex)
  : _parameter(*parameter), _startIndex(startIndex)
{}

void ExtractParametersCallback::run(
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
      oss << getText(*result.SourceManager, **i);
      if (i + 1 != e) { oss << ", "; }
    }
    _parameter = oss.str();
  }
}

ApplyParametersCallback::ApplyParametersCallback(
                          clang::tooling::Replacements& replacements,
                          const std::string& parameter)
  : _replacements(replacements), _parameter(parameter)
{}

void ApplyParametersCallback::run(
    const ast_matchers::MatchFinder::MatchResult& result)
{
  const FunctionDecl* funcDecl
    = result.Nodes.getDeclAs<FunctionDecl>("toDecl");
  if (funcDecl) {
    if (funcDecl->param_size() > 0) {
      const ParmVarDecl* lastParamDecl = funcDecl->getParamDecl(
                                            funcDecl->param_size() - 1);
      std::string lastParam = getText(*result.SourceManager,
                                      *lastParamDecl);

      _replacements.insert(Replacement(*result.SourceManager,
                                       lastParamDecl,
                                       lastParam + ", " + _parameter));
    } else {
      // look for the next token: a '('
      clang::SourceLocation insertLoc =
        clang::Lexer::findLocationAfterToken(funcDecl->getLocation(),
                                             clang::tok::l_paren,
                                             *result.SourceManager,
                                             clang::LangOptions(),
                       /*skip Whitespace? */ true);

      _replacements.insert(Replacement(*result.SourceManager,
                                       insertLoc, 0,
                                       _parameter));
    }
  }
}

} // namespace stooling

