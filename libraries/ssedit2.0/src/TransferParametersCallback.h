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

#ifndef TRANSFER_PARAMETERS_CALLBACK_H
#define TRANSFER_PARAMETERS_CALLBACK_H

namespace ssedit2 {

class ExtractParametersCallback
  : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
  ExtractParametersCallback(std::string* parameter, unsigned int startIndex);

  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult& result);

private:
  std::string&  _parameter;
  unsigned int  _startIndex;
};

class ApplyParametersCallback
  : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
  ApplyParametersCallback(clang::tooling::Replacements& replacements,
                          const std::string& parameter);

  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult& result);

private:
  clang::tooling::Replacements& _replacements;
  const std::string&            _parameter;
};

} // namespace ssedit2

#endif // TRANSFER_PARAMETERS_CALLBACK_H

