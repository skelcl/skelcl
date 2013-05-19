#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wc++11-long-long"

#include <clang/Frontend/FrontendActions.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/Refactoring.h>

#pragma GCC diagnostic pop

#include "CustomToolInvocation.h"

#ifndef REFACTORING_TOOL_H
#define REFACTORING_TOOL_H

namespace ssedit2 {

class RefactoringTool {

public:
  RefactoringTool();

  typedef std::set<clang::tooling::Replacement,
                   clang::tooling::Replacement::Less> Replacements;

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

  CustomToolInvocation      _invocation;
  Replacements              _replacements;
};

} // namespace ssedit2

#endif

