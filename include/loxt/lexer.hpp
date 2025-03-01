#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace loxt {

class TokenKind {
  enum class Kind : uint8_t {
#define LOXT_TOKEN(name) name,
#include "token_kinds.def"
  };
 public:
#define LOXT_TOKEN(name) \
  static constexpr auto name()->TokenKind { return TokenKind(Kind::name); }
#include "token_kinds.def"

  TokenKind() = delete;

  friend auto operator==(TokenKind lhs, TokenKind rhs) -> bool {
    return lhs.m_Kind == rhs.m_Kind;
  }

  friend auto operator!=(TokenKind lhs, TokenKind rhs) -> bool {
    return lhs.m_Kind != rhs.m_Kind;
  }

  [[nodiscard]] auto name() const -> const std::string&;

 private:
  constexpr explicit TokenKind(Kind kind) : m_Kind(kind) {}
  Kind m_Kind;
};

struct SourceLocation {
  int line;
  int column;
  std::string::const_iterator pos;

  auto operator++() -> SourceLocation& {
    if (*pos++ == '\n') {
      column = 1;
      ++line;
    } else {
      ++column;
    }
    return *this;
  }

  auto operator++(int) -> SourceLocation {
    auto old = *this;
    this->operator++();
    return old;
  }
};

using Identifier = unsigned int;
using Literal = unsigned int;

struct Token {
  TokenKind kind;
  union {
    Identifier identifier;
    Literal literal;
  };
  SourceLocation loc;

  Token(TokenKind in_kind, SourceLocation in_loc, unsigned int extra_id)
      : kind(in_kind), identifier(extra_id), loc(in_loc) {}
};

class TokenList {
 public:
  using Iterator = std::vector<Token>::iterator;
  using ConstIterator = std::vector<Token>::const_iterator;

  [[nodiscard]] auto begin() -> Iterator { return m_Tokens.begin(); }
  [[nodiscard]] auto begin() const -> ConstIterator { return m_Tokens.begin(); }
  [[nodiscard]] auto end() -> Iterator { return m_Tokens.end(); }
  [[nodiscard]] auto end() const -> ConstIterator { return m_Tokens.end(); }

  [[nodiscard]] auto identifier(Identifier ident) const
      -> const std::string_view& {
    return m_Identifiers[ident];
  }

  [[nodiscard]] auto string_literal(Literal literal) const
      -> const std::string& {
    return m_StringLiteral[literal];
  }

  [[nodiscard]] auto number_literal(Literal literal) const -> uint64_t {
    return m_NumberLiteral[literal];
  }

  [[nodiscard]] auto source() const -> const std::string& { return m_Source; }

  [[nodiscard]] auto to_string(const Token& token) const -> std::string;

  [[nodiscard]] auto has_error() const -> bool { return m_HasError; }

 private:
  explicit TokenList(const std::string& source) : m_Source(source) {}

  std::vector<Token> m_Tokens;
  std::unordered_map<std::string_view, Identifier> m_IdentifierMap;
  std::vector<std::string_view> m_Identifiers;
  std::vector<std::string> m_StringLiteral;
  std::vector<uint64_t> m_NumberLiteral;

  bool m_HasError = false;

  const std::string& m_Source;

  friend auto lex(const std::string& source) -> TokenList;
};

auto lex(const std::string& source) -> TokenList;

}  // namespace loxt