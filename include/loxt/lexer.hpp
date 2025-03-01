#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace loxt {

constexpr std::size_t Default_Block_Size = (1 << 15);

template <class T, std::size_t blockSize = Default_Block_Size>
class StaticVector {
  template <class S>
  using Block = std::vector<S>;

 public:
  class Iterator {
   public:
    Iterator() = delete;

    auto operator++() -> Iterator& {
      element++;
      if (element == block->size()) {
        block++;
        element = 0;
      }
      return *this;
    }

    auto operator*() -> T& { return *(block->begin() + element); }

    auto operator->() -> T* { return (block->begin() + element).operator->(); }

    auto operator<=>(const Iterator& rhs) const = default;

   private:
    Iterator(const typename std::vector<Block<T>>::iterator& blk,
             std::size_t elem)
        : block(blk), element(elem) {}

    friend class StaticVector<T>;

    typename std::vector<Block<T>>::iterator block;
    std::size_t element = 0;
  };

  StaticVector() { new_block(); }

  auto begin() -> Iterator { return {m_Blocks.begin(), 0}; }

  auto end() -> Iterator { return {m_Blocks.end(), 0}; }

  auto operator[](std::size_t idx) -> T& {
    return m_Blocks[idx / blockSize][idx % blockSize];
  }

  auto push_back(const T& element) {
    ++m_Size;
    m_Blocks.back().push_back(element);
    if (m_Blocks.back().size() == blockSize) {
      new_block();
    }
  }

  template <class... Args>
  auto emplace_back(Args&&... args) -> void {
    ++m_Size;
    m_Blocks.back().emplace_back(std::forward<Args>(args)...);
    if (m_Blocks.back().size() == blockSize) {
      new_block();
    }
  }

  [[nodiscard]] auto size() const -> std::size_t { return m_Size; }

 private:
  void new_block() {
    m_Blocks.emplace_back();
    m_Blocks.back().reserve(blockSize);
  }

  std::vector<Block<T>> m_Blocks;
  std::size_t m_Size = 0;
};

class TokenKind {
 public:
  enum class Kind : uint8_t {
#define LOXT_TOKEN(name) name,
#include "token_kinds.def"
  };
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

  [[nodiscard]] auto kind() const -> Kind { return m_Kind; }

 private:
  constexpr explicit TokenKind(Kind kind) : m_Kind(kind) {}
  Kind m_Kind;
};

struct SourceLocation {
  int line;
  int column;
  std::string::const_iterator pos;

  auto operator++() -> SourceLocation&;
  auto operator++(int) -> SourceLocation;
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
  using Iterator = StaticVector<Token>::Iterator;

  [[nodiscard]] auto begin() -> Iterator { return m_Tokens.begin(); }
  // [[nodiscard]] auto begin() const -> ConstIterator { return
  // m_Tokens.begin(); }
  [[nodiscard]] auto end() -> Iterator { return m_Tokens.end(); }
  // [[nodiscard]] auto end() const -> ConstIterator { return m_Tokens.end(); }

  [[nodiscard]] auto size() const -> std::size_t { return m_Tokens.size(); }

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

  StaticVector<Token> m_Tokens;
  std::unordered_map<std::string_view, Identifier> m_IdentifierMap;
  std::vector<std::string_view> m_Identifiers;
  std::vector<std::string> m_StringLiteral;
  std::vector<uint64_t> m_NumberLiteral;

  bool m_HasError = false;

  const std::string& m_Source;

  friend auto lex(const std::string& source) -> std::shared_ptr<TokenList>;
};

auto lex(const std::string& source) -> std::shared_ptr<TokenList>;

}  // namespace loxt