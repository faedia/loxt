#include "loxt/ast/expr.hpp"

#include <gtest/gtest.h>

#include "loxt/lexer.hpp"
#include "loxt/parser.hpp"

namespace loxt {

class PrinterVisitor : public Visitor {
 public:
  PrinterVisitor(const std::shared_ptr<TokenList>& tokens) : tokens_(tokens) {}

  void print(Expr& expr) { expr.accept(*this); }

  void visit(RootExpr& expr) override { expr.expr().accept(*this); }

  void visit(BinaryExpr& expr) override {
    std::cout << '(';
    std::cout << to_string(expr.op_kind()) << ' ';
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
    switch (expr.l_kind()) {
      case LiteralKind::String:
        std::cout << tokens_->string_literal(expr.literal());
        break;
      case LiteralKind::Number:
        std::cout << tokens_->number_literal(expr.literal());
        break;

      default:
        std::cout << "false";
    }
  }

  void visit(UnaryExpr& expr) override {
    std::cout << '(';
    std::cout << static_cast<int>(expr.op_kind()) << ' ';
    expr.expr().accept(*this);
    std::cout << ' ';
    std::cout << ')';
  }

 private:
  std::shared_ptr<TokenList> tokens_;
};
}  // namespace loxt

TEST(LexerTest, LexerTest1) {
  std::string str = "-12453 * (45)";
  auto toks = loxt::lex(str);
  loxt::ExprTree tree;

  tree.push_root(
      loxt::ExprData{loxt::ExprKind::Binary, {loxt::BinaryOpKind::Mul}});

  loxt::ExprData edata1{loxt::ExprKind::Literal, loxt::LiteralKind::Number,
                        (loxt::Literal)0};
  loxt::ExprData edata2{loxt::ExprKind::Literal, loxt::LiteralKind::Number,
                        (loxt::Literal)1};

  tree.push_child(tree.begin(), edata1);
  tree.push_child(tree.begin(), edata2);
  std::cout << "tree done\n";
  loxt::PrinterVisitor printer(toks);

  loxt::Expr rExpr{tree, tree.begin()};

  printer.print(rExpr);
  std::cout << '\n';
}

TEST(ParserTest, parserTest) {
  std::string str = "\"123\" == \"Hello\" != \"World\" == \"outer string\"";
  auto toks = loxt::lex(str);
  loxt::Parser parser{toks};
  parser.parse();
  loxt::PrinterVisitor printer(toks);
  loxt::Expr expr{parser.tree(), parser.tree().begin()};
  printer.print(expr);
  std::cout << '\n';
}