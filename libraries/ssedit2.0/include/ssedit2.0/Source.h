#ifndef SOURCE_H_
#define SOURCE_H_

#include <string>

namespace ssedit2 {

class RefactoringTool;

class Source {
public:
  Source(const std::string& source);

  ~Source();

  void transferParameters(const std::string& from,
                          unsigned int startIndex,
                          const std::string& to);

  void transferArguments(const std::string& from,
                         unsigned int startIndex,
                         const std::string& to);

  void renameFunction(const std::string& from, const std::string& to);

  void renameTypedef(const std::string& from, const std::string& to);

  const std::string& code();

private:
  Source();

  std::string       _source;
  RefactoringTool*  _tool;
};

} // namespace ssedit2

#endif // SOURCE_H_

