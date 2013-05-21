#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <clang/Basic/FileManager.h>
#include <clang/Driver/Compilation.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>

#pragma GCC diagnostic pop

#include <map>
#include <string>
#include <vector>

#ifndef CUSTOM_TOOL_INVOCATION_H
#define CUSTOM_TOOL_INVOCATION_H

namespace ssedit2 {

class CustomToolInvocation {
public:
  CustomToolInvocation(const std::string& code);

  ~CustomToolInvocation();

  bool run(clang::FrontendAction* action);

  clang::SourceManager& getSources();

private:
  void addFileMappingsTo(clang::SourceManager& SourceManager);

  bool runInvocation(clang::FrontendAction* action,
                     const char *BinaryName,
                     clang::driver::Compilation *Compilation,
                     clang::CompilerInvocation *Invocation);

  clang::CompilerInstance Compiler;
  std::string FileName;
  std::vector<std::string> CommandLine;
  clang::FileManager Files;
  std::string FileContent;
};

} // namespace ssedit2

#endif

