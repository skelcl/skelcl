#ifndef UTILITIES_H
#define UTILITIES_H

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

#include <utility>

namespace ssedit2 {

template <typename T>
static std::string getTextFromRange(const clang::SourceManager& sourceManager,
                                    const T& sourceRange)
{
  bool invalid = true;
  const char* text = sourceManager.getCharacterData(sourceRange.getBegin(),
                                                    &invalid);
  if (invalid) {
    return std::string();
  }
  std::pair<clang::FileID, unsigned> start =
      sourceManager.getDecomposedLoc(sourceRange.getBegin());
  std::pair<clang::FileID, unsigned> end =
      sourceManager.getDecomposedLoc(clang::Lexer::getLocForEndOfToken(
          sourceRange.getEnd(), 0, sourceManager, clang::LangOptions()));
  if (start.first != end.first) {
    // Start and end are in different files.
    return std::string();
  }
  if (end.second < start.second) {
    // Shuffling text with macros may cause this.
    return std::string();
  }
  return std::string(text, end.second - start.second);
}

template <typename T>
static std::string getText(const clang::SourceManager &sourceManager,
                           const T &node) {
  clang::SourceLocation startSpellingLocation =
      sourceManager.getSpellingLoc(node.getLocStart());
  clang::SourceLocation endSpellingLocation =
      sourceManager.getSpellingLoc(node.getLocEnd());
  if (!startSpellingLocation.isValid() || !endSpellingLocation.isValid()) {
    return std::string();
  }
  return getTextFromRange(sourceManager,
                          clang::SourceRange(startSpellingLocation,
                                             endSpellingLocation));
}

}

#endif // UTILITIES_H

