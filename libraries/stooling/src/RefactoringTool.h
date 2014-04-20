#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Weffc++"
#pragma GCC diagnostic ignored "-Wold-style-cast"
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

#include <clang/Frontend/FrontendActions.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/Refactoring.h>

#pragma GCC diagnostic pop

#ifndef REFACTORING_TOOL_H
#define REFACTORING_TOOL_H

namespace stooling {

class RefactoringTool {

public:
  RefactoringTool();

  ~RefactoringTool();

#if (LLVM_VERSION_MAJOR >= 3 && LLVM_VERSION_MINOR >= 4)
  typedef std::set<clang::tooling::Replacement> Replacements;
#else
  typedef std::set<clang::tooling::Replacement,
                   clang::tooling::Replacement::Less> Replacements;
#endif

  void run(const std::string& code,
           clang::tooling::FrontendActionFactory *actionFactory);

  std::string transform(const std::string& code,
                        clang::tooling::FrontendActionFactory *actionFactory);

  Replacements& replacements();

private:
  void prepareInvocation(const std::string& code,
                         clang::FrontendAction* action);

  bool applyAllReplacements(Replacements &replacements,
                            clang::Rewriter &rewriter);

  std::string writeReplacements(clang::Rewriter &rewriter);

  Replacements              _replacements;
};

} // namespace stooling

#endif

