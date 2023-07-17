#pragma once

#include <treeceratops/tree.hpp>

#include "../lexer.hpp"

namespace loxt {

enum class ExprKind { Root, Binary, Paren, Literal, Unary, Nil };

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

auto to_string(BinaryOpKind kind) -> std::string;

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

  explicit ExprData(ExprKind in_kind, BinaryOpKind b_op)
      : kind{in_kind}, bOp{b_op} {}
  explicit ExprData(ExprKind in_kind, LiteralKind l_kind, Literal lit)
      : kind{in_kind}, literalKind{l_kind}, literalVal(lit) {}
  explicit ExprData(ExprKind in_kind, LiteralKind l_kind, bool lit)
      : kind{in_kind}, literalKind{l_kind}, boolVal{lit} {}
  explicit ExprData(ExprKind in_kind, UnaryOpKind u_op)
      : kind{in_kind}, uOp{u_op} {}
  explicit ExprData(ExprKind in_kind) : kind{in_kind} {}
};

using ExprTree = treeceratops::tree<ExprData>;

class Expr;
class RootExpr;
class BinaryExpr;
class ParenExpr;
class NumberExpr;
class StringExpr;
class BoolExpr;
class UnaryExpr;
class NilExpr;

class Visitor {
 public:
  virtual void visit(RootExpr& expr) = 0;
  virtual void visit(BinaryExpr& expr) = 0;
  virtual void visit(ParenExpr& expr) = 0;
  virtual void visit(NumberExpr& expr) = 0;
  virtual void visit(StringExpr& expr) = 0;
  virtual void visit(BoolExpr& expr) = 0;
  virtual void visit(UnaryExpr& expr) = 0;
  virtual void visit(NilExpr& expr) = 0;
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

class NumberExpr : public Expr {
 public:
  [[nodiscard]] auto literal() const -> Literal { return node_->literalVal; }
};

class StringExpr : public Expr {
 public:
  [[nodiscard]] auto literal() const -> Literal { return node_->literalVal; }
};

class BoolExpr : public Expr {
 public:
  [[nodiscard]] auto literal() const -> bool { return node_->boolVal; }
};

class UnaryExpr : public Expr {
 public:
  [[nodiscard]] auto expr() const -> Expr {
    return {tree_, tree_.child<0>(node_)};
  }
  [[nodiscard]] auto op_kind() const -> UnaryOpKind { return node_->uOp; }
};

class NilExpr : public Expr {};

}  // namespace loxt