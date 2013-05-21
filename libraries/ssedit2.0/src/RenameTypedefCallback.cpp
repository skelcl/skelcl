#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

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

#include "RenameTypedefCallback.h"

using namespace clang;
using namespace clang::tooling;

namespace ssedit2 {

RenameTypedefCallback::RenameTypedefCallback(
    tooling::Replacements& replacements,
    const std::string& oldName,
    const std::string& newName)
  : _replacements(replacements), _oldName(oldName), _newName(newName)
{
}

void RenameTypedefCallback::run(
    const ast_matchers::MatchFinder::MatchResult& result)
{
  const TypedefNameDecl* typedefDecl
    = result.Nodes.getDeclAs<TypedefNameDecl>("decl");
  if (typedefDecl && typedefDecl->getName() == _oldName) {
    _replacements.insert(Replacement(*result.SourceManager, typedefDecl,
          "typedef "
          + typedefDecl->getUnderlyingType().getAsString()
          + " "
          + _newName));
  }
}

} // namespace ssedit2

