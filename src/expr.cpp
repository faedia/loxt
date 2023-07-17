#include <loxt/ast/expr.hpp>

namespace loxt {

auto to_string(BinaryOpKind kind) -> std::string {
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

  throw "Unknown Binary Op Kind";
}

void Expr::accept(Visitor& visitor) {
  switch (node_->kind) {
    case ExprKind::Root:
      visitor.visit(*static_cast<RootExpr*>(this));
      break;
    case ExprKind::Binary:
      visitor.visit(*static_cast<BinaryExpr*>(this));
      break;
    case ExprKind::Paren:
      visitor.visit(*static_cast<ParenExpr*>(this));
      break;
    case ExprKind::Literal:
      switch (node_->literalKind) {
        case LiteralKind::Number:
          visitor.visit(*static_cast<NumberExpr*>(this));
          break;
        case LiteralKind::String:
          visitor.visit(*static_cast<StringExpr*>(this));
          break;
        case LiteralKind::Bool:
          visitor.visit(*static_cast<BoolExpr*>(this));
          break;
      }
      break;
    case ExprKind::Unary:
      visitor.visit(*static_cast<UnaryExpr*>(this));
      break;
    case ExprKind::Nil:
      visitor.visit(*static_cast<NilExpr*>(this));
      break;
    default:
      break;
  }
}
}  // namespace loxt