#include <loxt/parser.hpp>

namespace loxt {

auto token_kind_bop_kind(TokenKind kind) -> BinaryOpKind {
  switch (kind.kind()) {
    case TokenKind::Kind::Or:
      return BinaryOpKind::Or;
    case TokenKind::Kind::And:
      return BinaryOpKind::And;
    case TokenKind::Kind::EqualEqual:
      return BinaryOpKind::Eq;
    case TokenKind::Kind::BangEqual:
      return BinaryOpKind::Neq;
    case TokenKind::Kind::Greater:
      return BinaryOpKind::Gt;
    case TokenKind::Kind::GreaterEqual:
      return BinaryOpKind::Ge;
    case TokenKind::Kind::Less:
      return BinaryOpKind::Lt;
    case TokenKind::Kind::LessEqual:
      return BinaryOpKind::Le;
    case TokenKind::Kind::Minus:
      return BinaryOpKind::Minus;
    case TokenKind::Kind::Plus:
      return BinaryOpKind::Add;
    case TokenKind::Kind::BackSlash:
      return BinaryOpKind::Div;
    case TokenKind::Kind::Asterisk:
      return BinaryOpKind::Mul;
    default:
      throw "Invalid binary operator";
  }
}

template <class T>
auto check(TokenList::Iterator& token, T kind) -> bool {
  return token->kind == kind;
}

template <class T, class... Targs>
auto check(TokenList::Iterator& token, T kind, Targs... args) -> bool {
  if (token->kind == kind) {
    return true;
  }
  return check(token, args...);
}

template <class T, class... Targs>
auto match(TokenList::Iterator& token, T kind, Targs... args) -> bool {
  if (check(token, kind, args...)) {
    ++token;
    return true;
  }
  return false;
}

Parser::Parser(std::shared_ptr<TokenList> tokens)
    : tokens_{std::move(tokens)}, current_{tokens_->begin()}, tree_{} {}

auto Parser::equality(ExprTree::iterator parent) -> ExprTree::iterator {
  auto lhs = primary(parent);
  while (check(current_, TokenKind::BangEqual(), TokenKind::EqualEqual())) {
    auto bop = token_kind_bop_kind(current_->kind);
    ++current_;
    tree_.push_child(parent, ExprData{ExprKind::Binary, bop});
    auto current = tree_.last_child(parent);
    tree_.make_parent(current, lhs);
    primary(current);
    lhs = current;
  }

  return lhs;
}

auto Parser::primary(ExprTree::iterator parent) -> ExprTree::iterator {
  if (check(current_, TokenKind::String())) {
    tree_.push_child(parent, ExprData{ExprKind::Literal, LiteralKind::String,
                                      current_->literal});
    ++current_;
    return tree_.last_child(parent);
  }
  throw "Failed to parse expr";
}

}  // namespace loxt