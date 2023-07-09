#pragma once

#include <treeceratops/tree.hpp>

#include "../lexer.hpp"

namespace loxt {

enum class ExprKind { Root, Binary, Paren, Literal, Unary };

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
inline auto to_string(BinaryOpKind kind) -> std::string {
  switch (kind) {
    case BinaryOpKind::Or:
      return "Or";
    case BinaryOpKind::And:
      return "And";
    case BinaryOpKind::Eq:
      return "Eq";
    case BinaryOpKind::Neq:
      return "Neq";
    case BinaryOpKind::Gt:
      return "Gt";
    case BinaryOpKind::Ge:
      return "Ge";
    case BinaryOpKind::Lt:
      return "Lt";
    case BinaryOpKind::Le:
      return "Le";
    case BinaryOpKind::Minus:
      return "Minus";
    case BinaryOpKind::Add:
      return "Add";
    case BinaryOpKind::Div:
      return "Div";
    case BinaryOpKind::Mul:
      return "Mul";
  }
}

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

  explicit ExprData(ExprKind inkind, BinaryOpKind b_op)
      : kind{inkind}, bOp{b_op} {}
  explicit ExprData(ExprKind inkind, LiteralKind l_kind, Literal lit)
      : kind{inkind}, literalKind{l_kind}, literalVal(lit) {}
  explicit ExprData(ExprKind inkind, LiteralKind l_kind, bool lit)
      : kind{inkind}, literalKind{l_kind}, boolVal{lit} {}
  explicit ExprData(ExprKind inkind, UnaryOpKind u_op)
      : kind{inkind}, uOp{u_op} {}
  explicit ExprData(ExprKind inkind) : kind{inkind} {}
};

using ExprTree = treeceratops::tree<ExprData>;

class Expr;
class RootExpr;
class BinaryExpr;
class ParenExpr;
class LiteralExpr;
class UnaryExpr;

class Visitor {
 public:
  virtual void visit(RootExpr& expr) = 0;
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

  [[nodiscard]] auto e_kind() const -> ExprKind { return node_->kind; }

 protected:
  ExprTree& tree_;
  ExprTree::iterator node_;
};

class RootExpr : public Expr {
 public:
  [[nodiscard]] auto expr() const -> Expr {
    return {tree_, tree_.child<0>(node_)};
  }
};

class BinaryExpr : public Expr {
 public:
  [[nodiscard]] auto lhs() const -> Expr {
    return {tree_, tree_.child<0>(node_)};
  }
  [[nodiscard]] auto rhs() const -> Expr {
    return {tree_, tree_.child<1>(node_)};
  }
  [[nodiscard]] auto op_kind() const -> BinaryOpKind { return node_->bOp; }
};

class ParenExpr : public Expr {
 public:
  [[nodiscard]] auto expr() const -> Expr {
    return {tree_, tree_.child<0>(node_)};
  }
};

class LiteralExpr : public Expr {
 public:
  [[nodiscard]] auto literal() const -> Literal { return node_->literalVal; }
  [[nodiscard]] auto l_kind() const -> LiteralKind {
    return node_->literalKind;
  }
};

class UnaryExpr : public Expr {
 public:
  [[nodiscard]] auto expr() const -> Expr {
    return {tree_, tree_.child<0>(node_)};
  }
  [[nodiscard]] auto op_kind() const -> UnaryOpKind { return node_->uOp; }
};

}  // namespace loxt