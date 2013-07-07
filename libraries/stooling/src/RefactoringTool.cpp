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
#pragma GCC diagnostic ignored "-Wshift-sign-overflow"
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wcast-align"
#pragma GCC diagnostic ignored "-Wduplicate-enum"

#include <llvm/Support/raw_os_ostream.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Tooling/Refactoring.h>
#include <clang/Tooling/Tooling.h>

#pragma GCC diagnostic pop

#include <sstream>
#include <iostream>

#include "CustomToolInvocation.h"
#include "RefactoringTool.h"

namespace stooling {

RefactoringTool::RefactoringTool()
  : _replacements()
{}

RefactoringTool::~RefactoringTool() {}

void RefactoringTool::run(const std::string& code,
                          clang::tooling::FrontendActionFactory *actionFactory)
{
  CustomToolInvocation(code).run(actionFactory->create());
}

std::string
RefactoringTool::transform(const std::string& code,
                           clang::tooling::FrontendActionFactory *actionFactory)
{
  CustomToolInvocation invocation(code);
  invocation.run(actionFactory->create());

  //create rewriter
  clang::LangOptions defaultLangOptions;
  clang::Rewriter rewriter(invocation.getSources(), defaultLangOptions);
  if (_replacements.empty()) {
    return code;
  }
  // apply replacements
  applyAllReplacements(_replacements, rewriter);
  // create replacements
  _replacements.clear();
  // write replacements to buffer
  return writeReplacements(rewriter);
}

RefactoringTool::Replacements& RefactoringTool::replacements()
{
  return _replacements;
}

bool RefactoringTool::applyAllReplacements(Replacements &replacements,
                                           clang::Rewriter &rewriter)
{
  bool result = true;
  for (auto& r : replacements) {
    if (r.isApplicable()) {
      result = r.apply(rewriter) && result;
    } else {
      result = false;
    }
  }
  return result;
}

std::string RefactoringTool::writeReplacements(clang::Rewriter &rewriter)
{
  std::ostringstream oss;
  for ( auto i  = rewriter.buffer_begin(),
             e  = rewriter.buffer_end();
             i != e;
           ++i ) {
    llvm::raw_os_ostream stream(oss);
    i->second.write(stream);
    stream.flush();
  }
  return oss.str();
}

} // namespace stooling

