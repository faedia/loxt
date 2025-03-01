#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace loxt {

class TokenKind {
  enum class Kind {
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

struct SourceLoc {
  int line;
  int column;
};

using Identifier = std::size_t;
using Literal = std::size_t;

struct Token {
  TokenKind kind;
  SourceLoc loc;
  union {
    Identifier identifier;
    Literal literal;
  };
};

class TokenList {
 public:
  using Iterator = std::vector<Token>::iterator;
  using ConstIterator = std::vector<Token>::const_iterator;

  [[nodiscard]] auto begin() -> Iterator { return m_Tokens.begin(); }
  [[nodiscard]] auto begin() const -> ConstIterator { return m_Tokens.begin(); }
  [[nodiscard]] auto end() -> Iterator { return m_Tokens.end(); }
  [[nodiscard]] auto end() const -> ConstIterator { return m_Tokens.end(); }

  [[nodiscard]] auto identifier(Identifier ident) const -> const std::string& {
    return m_Identifiers[ident];
  }

  [[nodiscard]] auto source() const -> const std::string& { return m_Source; }

 private:
  explicit TokenList(const std::string& source) : m_Source(source) {}

  std::vector<Token> m_Tokens;
  std::unordered_map<std::string, Identifier> m_IdentifierMap;
  std::vector<std::string> m_Identifiers;

  const std::string& m_Source;

  friend auto lex(const std::string& source) -> TokenList;
};

auto lex(const std::string& source) -> TokenList;

}  // namespace loxt