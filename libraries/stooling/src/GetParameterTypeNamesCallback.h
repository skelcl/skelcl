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
#include <vector>

#ifndef GET_PARAMETER_TYPE_NAMES_CALLBACK_H
#define GET_PARAMETER_TYPE_NAMES_CALLBACK_H

namespace stooling {

class GetParameterTypeNamesCallback
  : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
  GetParameterTypeNamesCallback();

  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult& result);

  std::vector<std::string> getParameterTypeNames() const;

private:
  std::vector<std::string> _parameterTypeNames;
};

} // namespace stooling

#endif // GET_PARAMETER_TYPE_NAMES_CALLBACK_H

