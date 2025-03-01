#include "ast/expr.hpp"
#include "lexer.hpp"

namespace loxt {

class Parser {
 public:
  explicit Parser(std::shared_ptr<TokenList> tokens);

  auto tree() -> ExprTree& { return tree_; }

  void parse() {
    tree_.push_root(ExprData{ExprKind::Root});
    auto root = tree_.begin();
    equality(root);
  }

 private:
  auto expression(ExprTree::iterator parent) -> ExprTree::iterator;
  auto equality(ExprTree::iterator parent) -> ExprTree::iterator;
  auto comparison(ExprTree::iterator parent) -> ExprTree::iterator;
  auto term(ExprTree::iterator parent) -> ExprTree::iterator;
  auto factor(ExprTree::iterator parent) -> ExprTree::iterator;
  auto unary(ExprTree::iterator parent) -> ExprTree::iterator;
  auto primary(ExprTree::iterator parent) -> ExprTree::iterator;

  std::shared_ptr<TokenList> tokens_;
  TokenList::Iterator current_;
  ExprTree tree_;
};

}  // namespace loxt