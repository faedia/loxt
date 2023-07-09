#include <loxt/ast/expr.hpp>

namespace loxt {
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