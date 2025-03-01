#include "loxt/ast/expr.hpp"

#include <gtest/gtest.h>

#include <print>

#include "loxt/lexer.hpp"
#include "loxt/parser.hpp"

namespace loxt {

class PrinterVisitor : public Visitor {
 public:
  explicit PrinterVisitor(const std::shared_ptr<TokenList>& tokens)
      : tokens_(tokens) {}

  void print(Expr& expr) { expr.accept(*this); }

  void visit(RootExpr& expr) override { expr.expr().accept(*this); }

  void visit(BinaryExpr& expr) override {
    std::println("{}BinaryExpr {}", std::string(4 * depth_, ' '),
                 to_string(expr.op_kind()));
    depth_++;
    expr.lhs().accept(*this);
    expr.rhs().accept(*this);
    depth_--;
  }

  void visit(ParenExpr& expr) override {
    std::println("{}ParenExpr", std::string(4 * depth_, ' '));
    depth_++;
    expr.expr().accept(*this);
    depth_--;
  }

  void visit(NumberExpr& expr) override {
    std::println("{}NumberExpr {}", std::string(4 * depth_, ' '),
                 tokens_->number_literal(expr.literal()));
  }

  void visit(StringExpr& expr) override {
    std::println("{}StringExpr {}", std::string(4 * depth_, ' '),
                 tokens_->string_literal(expr.literal()));
  }

  void visit(BoolExpr& expr) override {
    std::println("{}BoolExpr {}", std::string(4 * depth_, ' '), expr.literal());
  }

  void visit(UnaryExpr& expr) override {
    std::println("{}UnaryExpr {}", std::string(4 * depth_, ' '),
                 static_cast<int>(expr.op_kind()));
    depth_++;
    expr.expr().accept(*this);
    depth_--;
  }

  void visit(NilExpr& expr) override {
    std::println("{}NilExpr", std::string(4 * depth_, ' '));
  }

 private:
  std::shared_ptr<TokenList> tokens_;
  int depth_{0};
};
}  // namespace loxt

TEST(LexerTest, LexerTest1) {
  std::string str = "-12453 * (45)";
  auto toks = loxt::lex(str);
  loxt::ExprTree tree;

  tree.push_root(
      loxt::ExprData{loxt::ExprKind::Binary, loxt::BinaryOpKind::Mul});

  loxt::ExprData edata1{loxt::ExprKind::Literal, loxt::LiteralKind::Number,
                        static_cast<loxt::Literal>(0)};
  loxt::ExprData edata2{loxt::ExprKind::Literal, loxt::LiteralKind::Number,
                        static_cast<loxt::Literal>(1)};

  tree.push_child(tree.begin(), edata1);
  tree.push_child(tree.begin(), edata2);
  std::cout << "tree done\n";
  loxt::PrinterVisitor printer(toks);

  loxt::Expr rExpr{tree, tree.begin()};

  printer.print(rExpr);
}

TEST(ParserTest, parserTest) {
  std::string str =
      R"("123" == "Hello" != "World" == "outer string" + !"fsdd"))";
  auto toks = loxt::lex(str);
  loxt::Parser parser{toks};
  parser.parse();
  loxt::PrinterVisitor printer(toks);
  loxt::Expr expr{parser.tree(), parser.tree().begin()};
  printer.print(expr);
}

TEST(ParserTest, parserTest1) {
  try {
    std::string str = "1 + (2 == 5) / 7 == nil";
    auto toks = loxt::lex(str);
    loxt::Parser parser{toks};
    parser.parse();
    loxt::PrinterVisitor printer(toks);
    loxt::Expr expr{parser.tree(), parser.tree().begin()};
    printer.print(expr);
    SUCCEED() << "Completed without error.";
  } catch (const char* err) {
    std::cout << err;
  }
}
