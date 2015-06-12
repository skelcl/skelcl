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
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

#if ( (__GNUC__ >= 4 && __GNUC_MINOR__ >= 8 ) || (__GNUC__ >= 5) )
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif

#ifdef __clang__
# pragma GCC diagnostic ignored "-Wshift-sign-overflow"
# if (__clang_major__ >= 3 && __clang_minor__ >= 3)
#   pragma GCC diagnostic ignored "-Wduplicate-enum"
# endif
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

#include <stooling/Utilities.h>

#include "GetParameterTypeNamesCallback.h"

using namespace clang;
using namespace clang::tooling;

namespace stooling {

GetParameterTypeNamesCallback::GetParameterTypeNamesCallback()
  : _parameterTypeNames()
{
}

void GetParameterTypeNamesCallback::run(
    const ast_matchers::MatchFinder::MatchResult& result)
{
  auto funcDecl = result.Nodes.getDeclAs<FunctionDecl>("decl");
  if (funcDecl) {
    for (auto i  = funcDecl->param_begin(),
              e  = funcDecl->param_end();
              i != e;
            ++i) {
      _parameterTypeNames.push_back(
          // get unqualified, i.e. without const, etc. type
          (*i)->getType().getUnqualifiedType().getAsString()
          );
    }
  }
}

std::vector<std::string>
  GetParameterTypeNamesCallback::getParameterTypeNames() const
{
  return _parameterTypeNames;
}

} // namespace stooling

