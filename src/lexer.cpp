#include <loxt/lexer.hpp>

namespace loxt {

std::string TokenNames[] = {
#define LOXT_TOKEN(name) #name,
#include <loxt/token_kinds.def>
};

auto TokenKind::name() const -> const std::string& {
  return TokenNames[static_cast<int>(m_Kind)];
}

auto lex(const std::string& source) -> TokenList {
  TokenList list(source);

  list.m_IdentifierMap.emplace("main", 0);
  list.m_Identifiers.emplace_back("main");
  list.m_IdentifierMap.emplace("int", 1);
  list.m_Identifiers.emplace_back("int");

  Token tok{TokenKind::Identifier(), {1, 1}, {}};

  list.m_Tokens.emplace_back(tok);
  tok.kind = TokenKind::If();
  tok.loc = {1, 4};
  tok.identifier = 1;
  list.m_Tokens.emplace_back(tok);

  return list;
}

}  // namespace loxt
