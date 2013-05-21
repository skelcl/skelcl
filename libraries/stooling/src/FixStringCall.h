namespace {
  using namespace clang;
  using namespace clang::ast_matchers;
  using clang::tooling::Replacement;
  using clang::tooling::newFrontendActionFactory;

bool needParensAfterUnaryOperator(const clang::Expr &ExprNode)
{
  if (llvm::dyn_cast<clang::BinaryOperator>(&ExprNode) ||
      llvm::dyn_cast<clang::ConditionalOperator>(&ExprNode)) {
    return true;
  }
  if (const clang::CXXOperatorCallExpr *op =
      llvm::dyn_cast<clang::CXXOperatorCallExpr>(&ExprNode)) {
    return op->getNumArgs() == 2 &&
        op->getOperator() != clang::OO_PlusPlus &&
        op->getOperator() != clang::OO_MinusMinus &&
        op->getOperator() != clang::OO_Call &&
        op->getOperator() != clang::OO_Subscript;
  }
  return false;
}

template <typename T>
static std::string getText(const clang::SourceManager &SourceManager,
                           const T &Node) {
  clang::SourceLocation StartSpellingLocatino =
      SourceManager.getSpellingLoc(Node.getLocStart());
  clang::SourceLocation EndSpellingLocation =
      SourceManager.getSpellingLoc(Node.getLocEnd());
  if (!StartSpellingLocatino.isValid() || !EndSpellingLocation.isValid()) {
    return std::string();
  }
  bool Invalid = true;
  const char *Text =
      SourceManager.getCharacterData(StartSpellingLocatino, &Invalid);
  if (Invalid) {
    return std::string();
  }
  std::pair<clang::FileID, unsigned> Start =
      SourceManager.getDecomposedLoc(StartSpellingLocatino);
  std::pair<clang::FileID, unsigned> End =
      SourceManager.getDecomposedLoc(clang::Lexer::getLocForEndOfToken(
          EndSpellingLocation, 0, SourceManager, clang::LangOptions()));
  if (Start.first != End.first) {
    // Start and end are in different files.
    return std::string();
  }
  if (End.second < Start.second) {
    // Shuffling text with macros may cause this.
    return std::string();
  }
  return std::string(Text, End.second - Start.second);
}

std::string formatDereference(const clang::SourceManager &SourceManager,
                              const clang::Expr &ExprNode)
{
  if (const clang::UnaryOperator *Op =
      llvm::dyn_cast<clang::UnaryOperator>(&ExprNode)) {
    if (Op->getOpcode() == clang::UO_AddrOf) {
      // Strip leading '&'.
      return getText(SourceManager, *Op->getSubExpr()->IgnoreParens());
    }
  }
  const std::string Text = getText(SourceManager, ExprNode);
  if (Text.empty()) return std::string();
  // Add leading '*'.
  if (needParensAfterUnaryOperator(ExprNode)) {
    return std::string("*(") + Text + ")";
  }
  return std::string("*") + Text;
}

class FixCStrCall : public ast_matchers::MatchFinder::MatchCallback {
 public:
  FixCStrCall(tooling::Replacements& Replace)
      : Replace(&Replace) {}

  virtual void run(const ast_matchers::MatchFinder::MatchResult &Result) {
    const CallExpr *Call =
        Result.Nodes.getStmtAs<CallExpr>("call");
    const Expr *Arg =
        Result.Nodes.getStmtAs<Expr>("arg");
    const bool Arrow =
        Result.Nodes.getStmtAs<MemberExpr>("member")->isArrow();
    // Replace the "call" node with the "arg" node, prefixed with '*'
    // if the call was using '->' rather than '.'.
    const std::string ArgText = Arrow ?
        formatDereference(*Result.SourceManager, *Arg) :
        getText(*Result.SourceManager, *Arg);
    if (ArgText.empty()) return;

    Replace->insert(Replacement(*Result.SourceManager, Call, ArgText));
  }

 private:
  tooling::Replacements *Replace;
};
}

namespace ssedit2 {

void Source::fixStringCalls()
{
  using namespace clang;
  using namespace clang::ast_matchers;
  using clang::tooling::Replacement;
  using clang::tooling::newFrontendActionFactory;

  const char *StringConstructor =
    "::std::basic_string<char, std::char_traits<char>, std::allocator<char> >"
    "::basic_string";

  const char *StringCStrMethod =
    "::std::basic_string<char, std::char_traits<char>, std::allocator<char> >"
    "::c_str";
  
  ast_matchers::MatchFinder Finder;
  FixCStrCall Callback(_tool.replacements());
  Finder.addMatcher(
      constructExpr(
          hasDeclaration(methodDecl(hasName(StringConstructor))),
          argumentCountIs(2),
          // The first argument must have the form x.c_str() or p->c_str()
          // where the method is string::c_str().  We can use the copy
          // constructor of string instead (or the compiler might share
          // the string object).
          hasArgument(
              0,
              id("call", memberCallExpr(
                  callee(id("member", memberExpr())),
                  callee(methodDecl(hasName(StringCStrMethod))),
                  on(id("arg", expr()))))),
          // The second argument is the alloc object which must not be
          // present explicitly.
          hasArgument(
              1,
              defaultArgExpr())),
      &Callback);
  Finder.addMatcher(
      constructExpr(
          // Implicit constructors of these classes are overloaded
          // wrt. string types and they internally make a StringRef
          // referring to the argument.  Passing a string directly to
          // them is preferred to passing a char pointer.
          hasDeclaration(methodDecl(anyOf(
              hasName("::llvm::StringRef::StringRef"),
              hasName("::llvm::Twine::Twine")))),
          argumentCountIs(1),
          // The only argument must have the form x.c_str() or p->c_str()
          // where the method is string::c_str().  StringRef also has
          // a constructor from string which is more efficient (avoids
          // strlen), so we can construct StringRef from the string
          // directly.
          hasArgument(
              0,
              id("call", memberCallExpr(
                  callee(id("member", memberExpr())),
                  callee(methodDecl(hasName(StringCStrMethod))),
                  on(id("arg", expr())))))),
      &Callback);

  _source = _tool.transform(_source, newFrontendActionFactory(&Finder));
}

} // namespace ssedit2

