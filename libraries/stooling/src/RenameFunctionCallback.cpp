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

#if (__GNUC__ >= 4 && __GNUC_MINOR__ >= 8)
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

#include "RenameFunctionCallback.h"

using namespace clang;
using namespace clang::tooling;

namespace stooling {

RenameFunctionCallback::RenameFunctionCallback(
    tooling::Replacements& replacements,
    const std::string& newName)
  : _replacements(replacements), _newName(newName)
{
}

void RenameFunctionCallback::run(
    const ast_matchers::MatchFinder::MatchResult& result)
{
  renameFunctionDeclaration( result.Nodes.getDeclAs<FunctionDecl>("decl"),
                            *result.SourceManager);
  renameCallExpression( result.Nodes.getStmtAs<CallExpr>("call"),
                       *result.SourceManager);
}

void RenameFunctionCallback::renameFunctionDeclaration(
    const FunctionDecl* funcDecl,
    SourceManager& sourceManager)
{
  if (funcDecl) {
    _replacements.insert(Replacement(sourceManager,
                          CharSourceRange::getTokenRange(
                            SourceRange(funcDecl->getLocation())),
                          _newName));
  }
}

void RenameFunctionCallback::renameCallExpression(
    const CallExpr* callExpr,
    SourceManager& sourceManager)
{
  if (callExpr) {
    // travers children
    for ( clang::CallExpr::const_child_iterator
            i  = callExpr->child_begin(),
            e  = callExpr->child_end();
            i != e;
          ++i ) {
      // the name is wrapped in an implicit cast expression
      const ImplicitCastExpr* ice = dyn_cast<ImplicitCastExpr>(*i);
      if (ice) {
        for ( clang::CallExpr::const_child_iterator
                i2  = ice->child_begin(),
                e2  = ice->child_end();
                i2 != e2;
              ++i2 ) {
          const DeclRefExpr* declRefExpr = dyn_cast<DeclRefExpr>(*i2);
          if (declRefExpr) {
            _replacements.insert(Replacement(sourceManager,
                                  declRefExpr,
                                  _newName));
          }
        }
      }
    }
  }
}

} // namespace stooling

