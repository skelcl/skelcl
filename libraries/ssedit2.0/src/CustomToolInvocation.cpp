#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wc++11-long-long"

#include <llvm/Support/Host.h>
#include <clang/Basic/FileManager.h>
#include <clang/Driver/Compilation.h>
#include <clang/Driver/Driver.h>
#include <clang/Driver/Tool.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/FrontendDiagnostic.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>

#pragma GCC diagnostic pop

#include "CustomToolInvocation.h"

using namespace llvm;
using namespace clang;

namespace {

clang::driver::Driver *newDriver(clang::DiagnosticsEngine *Diagnostics,
                                        const char *BinaryName) {
  const std::string DefaultOutputName = "a.out";
  clang::driver::Driver *CompilerDriver = new clang::driver::Driver(
    BinaryName, llvm::sys::getDefaultTargetTriple(),
    DefaultOutputName, *Diagnostics);
  CompilerDriver->setTitle("clang_based_tool");
  return CompilerDriver;
}

const clang::driver::ArgStringList *getCC1Arguments(
    clang::DiagnosticsEngine *Diagnostics,
    clang::driver::Compilation *Compilation) {
  // We expect to get back exactly one Command job, if we didn't something
  // failed. Extract that job from the Compilation.
  const clang::driver::JobList &Jobs = Compilation->getJobs();
  if (Jobs.size() != 1 || !isa<clang::driver::Command>(*Jobs.begin())) {
    SmallString<256> error_msg;
    llvm::raw_svector_ostream error_stream(error_msg);
    Compilation->PrintJob(error_stream, Compilation->getJobs(), "; ", true);
    Diagnostics->Report(clang::diag::err_fe_expected_compiler_job)
        << error_stream.str();
    return NULL;
  }

  // The one job we find should be to invoke clang again.
  const clang::driver::Command *Cmd =
      cast<clang::driver::Command>(*Jobs.begin());
  if (StringRef(Cmd->getCreator().getName()) != "clang") {
    Diagnostics->Report(clang::diag::err_fe_expected_clang_command);
    return NULL;
  }

  return &Cmd->getArguments();
}

/// \brief Returns a clang build invocation initialized from the CC1 flags.
clang::CompilerInvocation *newInvocation(
    clang::DiagnosticsEngine *Diagnostics,
    const clang::driver::ArgStringList &CC1Args) {
  assert(!CC1Args.empty() && "Must at least contain the program name!");
  clang::CompilerInvocation *Invocation = new clang::CompilerInvocation;
  clang::CompilerInvocation::CreateFromArgs(
      *Invocation, CC1Args.data() + 1, CC1Args.data() + CC1Args.size(),
      *Diagnostics);
  Invocation->getFrontendOpts().DisableFree = false;
  return Invocation;
}

} // namespace

namespace ssedit2 {

CustomToolInvocation::CustomToolInvocation()
  : Compiler(), CommandLine(), ToolAction(), Files((FileSystemOptions()))
{
}

CustomToolInvocation::~CustomToolInvocation()
{
  Compiler.resetAndLeakFileManager();
  Files.clearStatCaches();
}

clang::SourceManager& CustomToolInvocation::getSources()
{
  return Compiler.getSourceManager();
}

void CustomToolInvocation::init(std::vector<std::string>& commandLine,
                                clang::FrontendAction *toolAction)
{
  CommandLine = commandLine;
  ToolAction = toolAction;
}

void CustomToolInvocation::mapVirtualFile(const std::string& FilePath,
                                          const std::string& Content) {
  SmallString<1024> PathStorage;
  llvm::sys::path::native(FilePath, PathStorage);
  MappedFileContents[PathStorage] = Content;
}

bool CustomToolInvocation::run() {
  std::vector<const char*> Argv;
  for (int I = 0, E = CommandLine.size(); I != E; ++I)
    Argv.push_back(CommandLine[I].c_str());
  const char *const BinaryName = Argv[0];
  IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts = new DiagnosticOptions();
  TextDiagnosticPrinter DiagnosticPrinter(
      llvm::errs(), &*DiagOpts);
  DiagnosticsEngine Diagnostics(
    IntrusiveRefCntPtr<clang::DiagnosticIDs>(new DiagnosticIDs()),
    &*DiagOpts, &DiagnosticPrinter, false);

  const OwningPtr<clang::driver::Driver> Driver(
      newDriver(&Diagnostics, BinaryName));
  // Since the input might only be virtual, don't check whether it exists.
  Driver->setCheckInputsExist(false);
  const OwningPtr<clang::driver::Compilation> Compilation(
      Driver->BuildCompilation(llvm::makeArrayRef(Argv)));
  const clang::driver::ArgStringList *const CC1Args = getCC1Arguments(
      &Diagnostics, Compilation.get());
  if (CC1Args == NULL) {
    return false;
  }
  OwningPtr<clang::CompilerInvocation> Invocation(
      newInvocation(&Diagnostics, *CC1Args));
  return runInvocation(BinaryName, Compilation.get(), Invocation.take());
}

bool CustomToolInvocation::runInvocation(
    const char * /*BinaryName*/,
    clang::driver::Compilation *Compilation,
    clang::CompilerInvocation *Invocation) {
  // Show the invocation, with -v.
  if (Invocation->getHeaderSearchOpts().Verbose) {
    llvm::errs() << "clang Invocation:\n";
    Compilation->PrintJob(llvm::errs(), Compilation->getJobs(), "\n", true);
    llvm::errs() << "\n";
  }

  // Create a compiler instance to handle the actual work.
  Compiler.setInvocation(Invocation);
  Compiler.setFileManager(&Files);
  // FIXME: What about LangOpts?

  // ToolAction can have lifetime requirements for Compiler or its members, and
  // we need to ensure it's deleted earlier than Compiler. So we pass it to an
  // OwningPtr declared after the Compiler variable.
  OwningPtr<FrontendAction> ScopedToolAction(ToolAction);

  // Create the compilers actual diagnostics engine.
  Compiler.createDiagnostics();
  if (!Compiler.hasDiagnostics())
    return false;

  Compiler.createSourceManager(Files);
  addFileMappingsTo(Compiler.getSourceManager());

  const bool Success = Compiler.ExecuteAction(*ScopedToolAction);

  return Success;
}

void CustomToolInvocation::addFileMappingsTo(SourceManager &Sources) {
  for (auto It = MappedFileContents.begin(), End = MappedFileContents.end();
       It != End; ++It) {
    // Inject the code as the given file name into the preprocessor options.
    const llvm::MemoryBuffer *Input =
        llvm::MemoryBuffer::getMemBuffer(It->getValue());
    // FIXME: figure out what '0' stands for.
    const FileEntry *FromFile = Files.getVirtualFile(
        It->getKey(), Input->getBufferSize(), 0);
    Sources.overrideFileContents(FromFile, Input);
  }
}

} // namespace ssedit2

