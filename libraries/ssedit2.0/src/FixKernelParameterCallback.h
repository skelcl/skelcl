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

#ifndef FIX_KERNEL_PARAMETER_CALLBACK_H
#define FIX_KERNEL_PARAMETER_CALLBACK_H

namespace ssedit2 {

class FixKernelParameterCallback
  : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
  FixKernelParameterCallback(clang::tooling::Replacements& replacements);

  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult& result);

private:
  void rewriteParameter(const clang::ParmVarDecl* param,
                        const std::string& dataType,
                        const std::string& paramName,
                        clang::SourceManager& sM);

  void adoptBody(const clang::FunctionDecl* funcDecl,
                 const std::string& fullType,
                 const std::string& paramName,
                 clang::SourceManager& sM);

  clang::tooling::Replacements& _replacements;
};

} // namespace ssedit2

#endif // FIX_KERNEL_PARAMETER_CALLBACK_H

