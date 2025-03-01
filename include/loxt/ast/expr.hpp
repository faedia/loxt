#pragma once

#include <treeceratops/tree.hpp>

#include "../lexer.hpp"

namespace loxt {

enum class ExprKind { Binary, Paren, Literal, Unary };

enum class BinaryOpKind {
  Or,
  And,
  Eq,
  Neq,
  Gt,
  Ge,
  Lt,
  Le,
  Minus,
  Add,
  Div,
  Mul
};

enum class UnaryOpKind { Not, Neg };

enum class LiteralKind { String, Number, Bool };

struct ExprData {
  ExprKind kind;
  union {
    struct {
      BinaryOpKind bOp;
    };
    struct {
      LiteralKind literalKind;
      union {
        bool boolVal;
        Literal literalVal;
      };
    };
    struct {
      UnaryOpKind uOp;
    };
  };
};

// using ExprTree = tree::Tree<ExprData>;

using ExprTree = treeceratops::tree<ExprData>;

// class Visitor {
//  public:
//   virtual void visitBinaryExpr(BinaryExpr& expr) = 0;
//   virtual void visitParenExpr(ParenExpr& expr) = 0;
//   virtual void visitStringExpr(StringExpr& expr) = 0;
//   virtual void visitNumberExpr(NumberExpr& expr) = 0;
//   virtual void visitBoolExpr(BoolExpr& expr) = 0;
//   virtual void visitUnaryExpr(UnaryExpr& expr) = 0;
// };

class Expr;
class BinaryExpr;
class NumberExpr;

class Visitor {
 public:
  virtual void visit(BinaryExpr& expr) = 0;
  // virtual void visit(ParenExpr& expr) = 0;
  // virtual void visit(StringExpr& expr) = 0;
  virtual void visit(NumberExpr& expr) = 0;
  // virtual void visit(BoolExpr& expr) = 0;
  // virtual void visit(UnaryExpr& expr) = 0;
};

class Expr {
 public:
  Expr(ExprTree& tree, const ExprTree::iterator& node)
      : tree_(tree), node_(node) {}

  void accept(Visitor& visitor);

 protected:
  ExprTree& tree_;
  ExprTree::iterator node_;
};

class BinaryExpr : public Expr {
 public:
  Expr lhs() const { return Expr(tree_, tree_.child<0>(node_)); }
  Expr rhs() const { return Expr(tree_, tree_.child<1>(node_)); }
  BinaryOpKind kind() { return node_->bOp; }
};

class NumberExpr : public Expr {
 public:
  Literal literal() { return node_->literalVal; }
};

void Expr::accept(Visitor& visitor) {
  switch (node_->kind) {
    case ExprKind::Binary:
      visitor.visit(*static_cast<BinaryExpr*>(this));
      break;
    // case Paren:
    //   visit(*static_cast<ParenExpr*>(this));
    case ExprKind::Literal:
      visitor.visit(*static_cast<NumberExpr*>(this));
      break;
    // case Unary:
    //   visit(*static_cast<UnaryExpr*>(this));
    default:
      break;
  }
}

// class ParenExpr : public Expr {
//  public:
//   ParenExpr(Expr& expr) : expr_(expr) {}

//   void accept(Visitor& visitor) { return visitor.visitParenExpr(*this); }

//   const Expr& expr() const { return expr_; }
//   Expr& expr() { return expr_; }

//  private:
//   Expr& expr_;
// };

// class LiteralExpr : public Expr {
//  public:
//   LiteralExpr(Literal literal) : literal_(literal) {}

//   virtual void accept(Visitor& visitor) = 0;

//   Literal literal() { return literal_; }

//  protected:
//   Literal literal_;
// };

// class StringExpr : public LiteralExpr {
//  public:
//   StringExpr(Literal literal) : LiteralExpr(literal) {}

//   void accept(Visitor& visitor) { return visitor.visitStringExpr(*this); }
// };

// class NumberExpr : public LiteralExpr {
//  public:
//   NumberExpr(Literal literal) : LiteralExpr(literal) {}

//   void accept(Visitor& visitor) { return visitor.visitNumberExpr(*this); }
// };

// class BoolExpr : public Expr {
//  public:
//   BoolExpr(bool value) : value_(value) {}

//   void accept(Visitor& visitor) { return visitor.visitBoolExpr(*this); }

//   bool value() { return value_; }

//  private:
//   bool value_;
// };

// class UnaryExpr : public Expr {
//  public:
//   enum class Kind { Not, Neg };

//   UnaryExpr(Kind kind, Expr& expr) : kind_(kind), expr_(expr) {}

//   void accept(Visitor& visitor) { return visitor.visitUnaryExpr(*this); }

//   auto kind() -> Kind { return kind_; }
//   auto expr() const -> const Expr& { return expr_; }
//   auto expr() -> Expr& { return expr_; }

//  private:
//   Kind kind_;
//   Expr& expr_;
// };
// class Expr;
// class BinaryExpr;
// class ParenExpr;
// class StringExpr;
// class NumberExpr;
// class BoolExpr;
// class UnaryExpr;

// class Visitor {
//  public:
//   virtual void visitBinaryExpr(BinaryExpr& expr) = 0;
//   virtual void visitParenExpr(ParenExpr& expr) = 0;
//   virtual void visitStringExpr(StringExpr& expr) = 0;
//   virtual void visitNumberExpr(NumberExpr& expr) = 0;
//   virtual void visitBoolExpr(BoolExpr& expr) = 0;
//   virtual void visitUnaryExpr(UnaryExpr& expr) = 0;
// };

// class Expr {
//  public:
//   virtual void accept(Visitor& visitor) = 0;
// };

}  // namespace loxt