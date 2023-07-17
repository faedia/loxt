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

auto token_kind_uop_kind(TokenKind kind) -> UnaryOpKind {
  switch (kind.kind()) {
    case TokenKind::Kind::Bang:
      return UnaryOpKind::Not;
    case TokenKind::Kind::Minus:
      return UnaryOpKind::Neg;
    default:
      throw "Invalid binary operator";
  }
}

template <class T>
auto check(TokenList::Iterator& token, T kind) -> bool {
  return token->kind == kind;
}

template <class T, class... TArgs>
auto check(TokenList::Iterator& token, T kind, TArgs... args) -> bool {
  if (token->kind == kind) {
    return true;
  }
  return check(token, args...);
}

Parser::Parser(std::shared_ptr<TokenList> tokens)
    : tokens_{std::move(tokens)}, current_{tokens_->begin()}, tree_{} {}

auto Parser::expression(ExprTree::iterator parent) -> ExprTree::iterator {
  return equality(parent);
}

auto Parser::equality(ExprTree::iterator parent) -> ExprTree::iterator {
  auto lhs = comparison(parent);
  while (check(current_, TokenKind::BangEqual(), TokenKind::EqualEqual())) {
    auto bop = token_kind_bop_kind(current_->kind);
    ++current_;
    tree_.push_child(parent, ExprData{ExprKind::Binary, bop});
    auto current = tree_.last_child(parent);
    tree_.make_parent(current, lhs);
    comparison(current);
    lhs = current;
  }

  return lhs;
}

auto Parser::comparison(ExprTree::iterator parent) -> ExprTree::iterator {
  auto lhs = term(parent);

  while (check(current_, TokenKind::Greater(), TokenKind::GreaterEqual(),
               TokenKind::Less(), TokenKind::LessEqual())) {
    auto bop = token_kind_bop_kind(current_->kind);
    ++current_;
    tree_.push_child(parent, ExprData{ExprKind::Binary, bop});
    auto current = tree_.last_child(parent);
    tree_.make_parent(current, lhs);
    term(current);
    lhs = current;
  }

  return lhs;
}

auto Parser::term(ExprTree::iterator parent) -> ExprTree::iterator {
  auto lhs = factor(parent);

  while (check(current_, TokenKind::Minus(), TokenKind::Plus())) {
    auto bop = token_kind_bop_kind(current_->kind);
    ++current_;
    tree_.push_child(parent, ExprData{ExprKind::Binary, bop});
    auto current = tree_.last_child(parent);
    tree_.make_parent(current, lhs);
    factor(current);
    lhs = current;
  }

  return lhs;
}

auto Parser::factor(ExprTree::iterator parent) -> ExprTree::iterator {
  auto lhs = unary(parent);

  while (check(current_, TokenKind::BackSlash(), TokenKind::Asterisk())) {
    auto bop = token_kind_bop_kind(current_->kind);
    ++current_;
    tree_.push_child(parent, ExprData{ExprKind::Binary, bop});
    auto current = tree_.last_child(parent);
    tree_.make_parent(current, lhs);
    unary(current);
    lhs = current;
  }

  return lhs;
}

auto Parser::unary(ExprTree::iterator parent) -> ExprTree::iterator {
  if (check(current_, TokenKind::Bang(), TokenKind::Minus())) {
    auto uop = token_kind_uop_kind(current_->kind);
    ++current_;
    tree_.push_child(parent, ExprData{ExprKind::Unary, uop});
    parent = tree_.last_child(parent);
    unary(parent);
    return parent;
  }
  return primary(parent);
}

auto Parser::primary(ExprTree::iterator parent) -> ExprTree::iterator {
  if (check(current_, TokenKind::Nil())) {
    tree_.push_child(parent, ExprData{ExprKind::Nil});
    ++current_;
    return tree_.last_child(parent);
  }
  if (check(current_, TokenKind::False())) {
    tree_.push_child(parent,
                     ExprData{ExprKind::Literal, LiteralKind::Bool, false});
    ++current_;
    return tree_.last_child(parent);
  }
  if (check(current_, TokenKind::True())) {
    tree_.push_child(parent,
                     ExprData{ExprKind::Literal, LiteralKind::Bool, true});
    ++current_;
    return tree_.last_child(parent);
  }
  if (check(current_, TokenKind::Number())) {
    tree_.push_child(parent, ExprData{ExprKind::Literal, LiteralKind::Number,
                                      current_->literal});
    ++current_;
    return tree_.last_child(parent);
  }
  if (check(current_, TokenKind::String())) {
    tree_.push_child(parent, ExprData{ExprKind::Literal, LiteralKind::String,
                                      current_->literal});
    ++current_;
    return tree_.last_child(parent);
  }

  if (check(current_, TokenKind::LeftParen())) {
    ++current_;
    tree_.push_child(parent, ExprData{ExprKind::Paren});
    parent = tree_.last_child(parent);
    expression(parent);
    if (check(current_, TokenKind::RightParen())) {
      ++current_;
      return parent;
    }
    throw "hanging parent";
  }

  throw "Failed to parse expr";
}

}  // namespace loxt