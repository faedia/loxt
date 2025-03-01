#include <cctype>
#include <iostream>
#include <loxt/lexer.hpp>
#include <unordered_set>

namespace loxt {

std::string TokenNames[] = {
#define LOXT_TOKEN(name) #name,
#include <loxt/token_kinds.def>
};

std::unordered_map<std::string_view, TokenKind> Keywords = {
#define LOXT_KEYWORD_TOKEN(name, spelling) {spelling, TokenKind::name()},
#include <loxt/token_kinds.def>
};

auto TokenKind::name() const -> const std::string& {
  return TokenNames[static_cast<int>(m_Kind)];
}

auto match(char expected, const std::string& source, SourceLocation& loc)
    -> bool {
  if (loc.pos == source.end()) {
    return false;
  }
  if (*loc.pos != expected) {
    return false;
  }
  ++loc;
  return true;
}

auto lex(const std::string& source) -> TokenList {
  TokenList list(source);

  SourceLocation loc{1, 1, source.begin(), source.begin()};

  while (loc.pos != source.end()) {
    SourceLocation start_loc = loc;
    char chr = *(loc++).pos;
    switch (chr) {
      case '(':
        list.m_Tokens.emplace_back(Token{TokenKind::LeftParen(), start_loc, 0});
        break;
      case ')':
        list.m_Tokens.emplace_back(
            Token{TokenKind::RightParen(), start_loc, 0});
        break;
      case '{':
        list.m_Tokens.emplace_back(Token{TokenKind::LeftBrace(), start_loc, 0});
        break;
      case '}':
        list.m_Tokens.emplace_back(
            Token{TokenKind::RightBrace(), start_loc, 0});
        break;
      case ',':
        list.m_Tokens.emplace_back(Token{TokenKind::Comma(), start_loc, 0});
        break;
      case '.':
        list.m_Tokens.emplace_back(Token{TokenKind::Period(), start_loc, 0});
        break;
      case '-':
        list.m_Tokens.emplace_back(Token{TokenKind::Minus(), start_loc, 0});
        break;
      case '+':
        list.m_Tokens.emplace_back(Token{TokenKind::Plus(), start_loc, 0});
        break;
      case ';':
        list.m_Tokens.emplace_back(Token{TokenKind::SemiColon(), start_loc, 0});
        break;
      case '*':
        list.m_Tokens.emplace_back(Token{TokenKind::Asterisk(), start_loc, 0});
        break;
      case '!':
        list.m_Tokens.emplace_back(Token{match('=', source, loc)
                                             ? TokenKind::BangEqual()
                                             : TokenKind::Bang(),
                                         start_loc, 0});
        break;
      case '=':
        list.m_Tokens.emplace_back(Token{match('=', source, loc)
                                             ? TokenKind::EqualEqual()
                                             : TokenKind::Equal(),
                                         start_loc, 0});
        break;
      case '<':
        list.m_Tokens.emplace_back(Token{match('=', source, loc)
                                             ? TokenKind::LessEqual()
                                             : TokenKind::Less(),
                                         start_loc, 0});
        break;
      case '>':
        list.m_Tokens.emplace_back(Token{match('=', source, loc)
                                             ? TokenKind::GreaterEqual()
                                             : TokenKind::Greater(),
                                         start_loc, 0});
        break;
      case '/':
        if (match('/', source, loc)) {
          while (*loc.pos != '\n' && loc.pos != source.end()) {
            ++loc;
          }
        } else {
          list.m_Tokens.emplace_back(
              Token{TokenKind::BackSlash(), start_loc, 0});
        }
        break;
      case '"': {
        while (*loc.pos != '"' && loc.pos != source.end()) {
          ++loc;
        }

        if (loc.pos == source.end()) {
          std::cout << "STRING ERROR" << std::endl;
          return list;
        }

        ++loc;
        std::string_view str(start_loc.pos + 1, loc.pos - 1);
        std::cout << str << std::endl;
        list.m_Tokens.emplace_back(Token{TokenKind::String(), start_loc, 0});
      } break;
      default:
        if (static_cast<bool>(std::isalpha(chr))) {
          while (static_cast<bool>(std::isalnum(*loc.pos))) {
            ++loc;
          }
          std::string_view identifier_str{start_loc.pos, loc.pos};
          auto keyword_iter = Keywords.find(identifier_str);
          if (keyword_iter == Keywords.end()) {
            auto identifer_iter = list.m_IdentifierMap.find(identifier_str);
            Identifier identifer = list.m_Identifiers.size();
            if (identifer_iter == list.m_IdentifierMap.end()) {
              list.m_IdentifierMap.emplace(identifier_str, identifer);
              list.m_Identifiers.emplace_back(identifier_str);
            } else {
              identifer = identifer_iter->second;
            }
            list.m_Tokens.emplace_back(
                Token{TokenKind::Identifier(), start_loc, identifer});
          } else {
            list.m_Tokens.emplace_back(
                Token{keyword_iter->second, start_loc, 0});
          }
        } else if (std::isspace(chr) == 0) {
          std::cout << "ERROR" << std::endl;
          return list;
        }
    }
  }

  return list;
}

}  // namespace loxt
