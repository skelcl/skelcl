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

#ifndef RENAME_FUNCTION_CALLBACK_H
#define RENAME_FUNCTION_CALLBACK_H

namespace ssedit2 {

class RenameFunctionCallback
  : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
  RenameFunctionCallback(clang::tooling::Replacements& replacements,
                         const std::string& newName);

  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult& result);

private:
  void renameFunctionDeclaration(const clang::FunctionDecl* funcDecl,
                                 clang::SourceManager& sourceManager);

  void renameCallExpression(const clang::CallExpr* callExpr,
                            clang::SourceManager& sourceManager);

  clang::tooling::Replacements& _replacements;
  const std::string&            _newName;
};

} // namespace ssedit2

#endif // RENAME_FUNCTION_CALLBACK_H

