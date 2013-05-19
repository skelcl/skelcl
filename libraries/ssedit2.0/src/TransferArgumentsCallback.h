#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wc++11-long-long"

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

#ifndef TRANSFER_ARGUMENTS_CALLBACK_H
#define TRANSFER_ARGUMENTS_CALLBACK_H

namespace ssedit2 {

class ExtractArgumentsCallback
  : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
  ExtractArgumentsCallback(std::string* arguments, unsigned int startIndex);

  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult& result);

private:
  std::string&  _arguments;
  unsigned int  _startIndex;
};

class ApplyArgumentsCallback
  : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
  ApplyArgumentsCallback(clang::tooling::Replacements& replacements,
                          const std::string& arguments);

  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult& result);

private:
  clang::tooling::Replacements& _replacements;
  const std::string&            _arguments;
};

} // namespace ssedit2

#endif // TRANSFER_ARGUMENTS_CALLBACK_H

