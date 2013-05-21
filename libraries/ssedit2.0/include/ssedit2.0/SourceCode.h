#ifndef SSEDIT2_SOURCE_CODE_H_
#define SSEDIT2_SOURCE_CODE_H_

#include <string>

namespace ssedit2 {

class RefactoringTool;

class SourceCode {
public:
  SourceCode(const std::string& source);

  SourceCode(SourceCode&& rhs);

  SourceCode& operator=(SourceCode&& rhs);
  
  ~SourceCode();

  void transferParameters(const std::string& from,
                          unsigned int startIndex,
                          const std::string& to);

  void transferArguments(const std::string& from,
                         unsigned int startIndex,
                         const std::string& to);

  void renameFunction(const std::string& from, const std::string& to);

  void renameTypedef(const std::string& from, const std::string& to);

  void redefineTypedef(const std::string& typedefName,
                       const std::string& typeName);

  void fixKernelParameter(const std::string& kernel);

  const std::string& code();

private:
  SourceCode();

  std::string       _source;
  RefactoringTool*  _tool;
};

} // namespace ssedit2

#endif // SSEDIT2_SOURCE_CODE_H_

