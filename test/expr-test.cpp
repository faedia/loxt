#include "loxt/ast/expr.hpp"

#include <gtest/gtest.h>

#include "loxt/lexer.hpp"

namespace loxt {

class PrinterVisitor : public Visitor {
 public:
  PrinterVisitor(TokenList& tokens) : tokens_(tokens) {}

  void print(Expr& expr) { expr.accept(*this); }

  void visit(BinaryExpr& expr) override {
    std::cout << '(';
    std::cout << static_cast<int>(expr.op_kind()) << ' ';
    expr.lhs().accept(*this);
    std::cout << ' ';
    expr.rhs().accept(*this);
    std::cout << ')';
  }

  void visit(ParenExpr& expr) override {
    std::cout << '(';
    expr.expr().accept(*this);
    std::cout << ')';
  }

  void visit(LiteralExpr& expr) override {
    std::cout << tokens_.number_literal(expr.literal());
  }

  void visit(UnaryExpr& expr) override {
    std::cout << '(';
    std::cout << static_cast<int>(expr.op_kind()) << ' ';
    expr.expr().accept(*this);
    std::cout << ' ';
    std::cout << ')';
  }

 private:
  TokenList& tokens_;
};
}  // namespace loxt

TEST(LexerTest, LexerTest1) {
  std::string str = "-12453 * (45)";
  auto toks = loxt::lex(str);
  loxt::ExprTree tree;

  tree.push_root({loxt::ExprKind::Binary, {loxt::BinaryOpKind::Mul}});

  loxt::ExprData edata1;
  edata1.kind = loxt::ExprKind::Literal;
  edata1.literalKind = loxt::LiteralKind::Number;
  edata1.literalVal = 0;

  loxt::ExprData edata2;
  edata2.kind = loxt::ExprKind::Literal;
  edata2.literalKind = loxt::LiteralKind::Number;
  edata2.literalVal = 1;

  tree.push_child(tree.begin(), edata1);
  tree.push_child(tree.begin(), edata2);
  std::cout << "tree done\n";
  loxt::PrinterVisitor printer(toks);

  loxt::Expr rExpr{tree, tree.begin()};

  printer.print(rExpr);
  std::cout << '\n';
}
