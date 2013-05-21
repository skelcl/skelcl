#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Weffc++"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-promo"

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

#include <iostream>
#include <string>
#include <sstream>

#include "FixKernelParameterCallback.h"
#include "Utilities.h"

using namespace clang;
using namespace clang::tooling;

namespace stooling {

FixKernelParameterCallback::FixKernelParameterCallback(
    tooling::Replacements& replacements)
  : _replacements(replacements)
{
}

void FixKernelParameterCallback::run(
    const ast_matchers::MatchFinder::MatchResult& result)
{
  auto funcDecl = result.Nodes.getDeclAs<FunctionDecl>("decl");
  if (funcDecl) {
    // TODO: find a better way to check if this is a kernel
    if (   getText(*result.SourceManager, *funcDecl).find("kernel")
        == std::string::npos) { return; }
    // search all parameters ...
    for ( auto param  = funcDecl->param_begin(),
               last   = funcDecl->param_end();
               param != last;
             ++param ) {
      // ... and look for a type ending in _matrix_t
      auto fullType = (*param)->getOriginalType().getAsString();
      auto pos = fullType.rfind("_matrix_t");
      if ( pos != std::string::npos) {
        // if found transform this parameter into two and adopt the body
        // accordingly
        auto dataType = fullType.substr(0, pos);
        auto paramName = (*param)->getName().str();
        rewriteParameter(*param, dataType, paramName, *result.SourceManager);
        adoptBody(funcDecl, fullType, paramName, *result.SourceManager);
      }
    }
  }
}

void FixKernelParameterCallback::rewriteParameter(
    const clang::ParmVarDecl* param,
    const std::string& dataType,
    const std::string& paramName,
    SourceManager& sM)
{
  std::ostringstream oss;
  oss << "__global " << dataType << "* ";
  oss << paramName << "_data";
  oss << ", unsigned int " << paramName << "_col_count";

  _replacements.insert(Replacement(sM, param, oss.str()));
}

void FixKernelParameterCallback::adoptBody(const FunctionDecl* funcDecl,
                                           const std::string& fullType,
                                           const std::string& paramName,
                                           SourceManager& sM)
{
  std::ostringstream oss;
  oss << fullType << " " << paramName << ";\n";
  oss << paramName << ".data = " << paramName << "_data;\n";
  oss << paramName << ".col_count = " << paramName << "_col_count;\n";

  auto body = dyn_cast<CompoundStmt>(funcDecl->getBody());
  _replacements.insert(
      Replacement(sM, (*(body->body_begin()))->getLocStart(), 0, oss.str()));
}

} // namespace stooling

