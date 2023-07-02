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

using ExprTree = treeceratops::tree<ExprData>;

class Expr;
class BinaryExpr;
class ParenExpr;
class LiteralExpr;
class UnaryExpr;

class Visitor {
 public:
  virtual void visit(BinaryExpr& expr) = 0;
  virtual void visit(ParenExpr& expr) = 0;
  virtual void visit(LiteralExpr& expr) = 0;
  virtual void visit(UnaryExpr& expr) = 0;
};

class Expr {
 public:
  Expr(ExprTree& tree, const ExprTree::iterator& node)
      : tree_(tree), node_(node) {}

  void accept(Visitor& visitor);

  ExprKind e_kind() { return node_->kind; }

 protected:
  ExprTree& tree_;
  ExprTree::iterator node_;
};

class BinaryExpr : public Expr {
 public:
  Expr lhs() const { return Expr(tree_, tree_.child<0>(node_)); }
  Expr rhs() const { return Expr(tree_, tree_.child<1>(node_)); }
  BinaryOpKind op_kind() { return node_->bOp; }
};

class ParenExpr : public Expr {
 public:
  Expr expr() const { return Expr(tree_, tree_.child<0>(node_)); }
};

class LiteralExpr : public Expr {
 public:
  Literal literal() const { return node_->literalVal; }
  LiteralKind l_kind() const { return node_->literalKind; }
};

class UnaryExpr : public Expr {
 public:
  Expr expr() const { return Expr(tree_, tree_.child<0>(node_)); }
  UnaryOpKind op_kind() const { return node_->uOp; }
};

void Expr::accept(Visitor& visitor) {
  switch (node_->kind) {
    case ExprKind::Binary:
      visitor.visit(*static_cast<BinaryExpr*>(this));
      break;
    case ExprKind::Paren:
      visitor.visit(*static_cast<ParenExpr*>(this));
      break;
    case ExprKind::Literal:
      visitor.visit(*static_cast<LiteralExpr*>(this));
      break;
    case ExprKind::Unary:
      visitor.visit(*static_cast<UnaryExpr*>(this));
      break;
    default:
      break;
  }
}

}  // namespace loxt