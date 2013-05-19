#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wc++11-long-long"

#include <clang/Basic/FileManager.h>
#include <clang/Driver/Compilation.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>

#pragma GCC diagnostic pop

#include <string>
#include <vector>

#ifndef CUSTOM_TOOL_INVOCATION_H
#define CUSTOM_TOOL_INVOCATION_H

namespace ssedit2 {

class CustomToolInvocation {
public:
  CustomToolInvocation();

  ~CustomToolInvocation();

  void init(std::vector<std::string>& CommandLine,
            clang::FrontendAction *ToolAction);

  void mapVirtualFile(const std::string& FilePath,
                      const std::string& Content);
  bool run();

  clang::SourceManager& getSources();

private:
  void addFileMappingsTo(clang::SourceManager& SourceManager);

  bool runInvocation(const char *BinaryName,
                     clang::driver::Compilation *Compilation,
                     clang::CompilerInvocation *Invocation);

  clang::CompilerInstance Compiler;
  std::vector<std::string> CommandLine;
  clang::FrontendAction* ToolAction; // was smart pointer
  clang::FileManager Files;
  // Maps <file name> -> <file content>.
  llvm::StringMap<llvm::StringRef> MappedFileContents;
};

} // namespace ssedit2

#endif

