#include <fmt/core.h>

#include <cctype>
#include <iostream>
#include <loxt/lexer.hpp>
#include <unordered_set>

namespace loxt {

auto report(SourceLocation loc, const std::string& error_text) -> void {
  fmt::print("{}:{}: Error: {}\n", loc.line, loc.column, error_text);
}

const std::string TokenNames[] = {
#define LOXT_TOKEN(name) #name,
#include <loxt/token_kinds.def>
};

const std::unordered_map<std::string_view, const TokenKind> Keywords = {
#define LOXT_KEYWORD_TOKEN(name, spelling) {spelling, TokenKind::name()},
#include <loxt/token_kinds.def>
};

auto TokenKind::name() const -> const std::string& {
  return TokenNames[static_cast<int>(m_Kind)];
}

auto TokenList::to_string(const Token& token) const -> std::string {
  std::string str = "{kind: " + token.kind.name() +
                    ", line: " + std::to_string(token.loc.line) +
                    ", column: " + std::to_string(token.loc.column);

  if (token.kind == TokenKind::Identifier()) {
    str += ", identifier: " + std::string(identifier(token.identifier));
  }

  if (token.kind == TokenKind::String()) {
    str += ", literal: \"" + string_literal(token.literal) + "\"";
  }

  if (token.kind == TokenKind::Number()) {
    str += ", literal: " + std::to_string(number_literal(token.literal));
  }

  str += "}";
  return str;
}

inline auto match(char expected, const std::string& source, SourceLocation& loc)
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
  std::cout << sizeof(Token) << std::endl;
  TokenList list(source);

  SourceLocation loc{1, 1, source.begin()};

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
          while (loc.pos != source.end() && *loc.pos != '\n') {
            ++loc;
          }
        } else {
          list.m_Tokens.emplace_back(
              Token{TokenKind::BackSlash(), start_loc, 0});
        }
        break;
      case '"':
        while (loc.pos != source.end() && *loc.pos != '"') {
          ++loc;
        }

        if (loc.pos == source.end()) {
          list.m_Tokens.emplace_back(Token{TokenKind::Error(), start_loc, 0});
          report(start_loc, "String is unterminated");
          list.m_HasError = true;
        } else {
          ++loc;
          list.m_Tokens.emplace_back(
              Token{TokenKind::String(), start_loc,
                    static_cast<Literal>(list.m_StringLiteral.size())});
          list.m_StringLiteral.emplace_back(start_loc.pos + 1, loc.pos - 1);
        }
        break;
      default:
        if (static_cast<bool>(std::isdigit(chr))) {
          while (loc.pos != source.end() &&
                 static_cast<bool>(std::isdigit(*loc.pos))) {
            ++loc;
          }

          list.m_Tokens.emplace_back(
              Token{TokenKind::Number(), start_loc,
                    static_cast<Literal>(list.m_NumberLiteral.size())});
          list.m_NumberLiteral.emplace_back(
              std::stoull(std::string(start_loc.pos, loc.pos)));
        } else if (static_cast<bool>(std::isalpha(chr))) {
          while (loc.pos != source.end() &&
                 static_cast<bool>(std::isalnum(*loc.pos))) {
            ++loc;
          }

          std::string_view identifier_str{start_loc.pos, loc.pos};
          auto keyword_iter = Keywords.find(identifier_str);
          if (keyword_iter == Keywords.end()) {
            auto identifier_iter = list.m_IdentifierMap.find(identifier_str);
            Identifier identifier = list.m_Identifiers.size();
            if (identifier_iter == list.m_IdentifierMap.end()) {
              list.m_IdentifierMap.emplace(identifier_str, identifier);
              list.m_Identifiers.emplace_back(identifier_str);
            } else {
              identifier = identifier_iter->second;
            }
            list.m_Tokens.emplace_back(
                Token{TokenKind::Identifier(), start_loc, identifier});
          } else {
            list.m_Tokens.emplace_back(
                Token{keyword_iter->second, start_loc, 0});
          }
        } else if (std::isspace(chr) == 0) {
          list.m_Tokens.emplace_back(Token{TokenKind::Error(), start_loc, 0});
          report(start_loc, fmt::format("Unrecognized character '{}'", chr));
          list.m_HasError = true;
        }
    }
  }
  list.m_Tokens.emplace_back(Token{TokenKind::Eof(), loc, 0});
  list.m_Tokens.shrink_to_fit();
  list.m_Identifiers.shrink_to_fit();
  list.m_StringLiteral.shrink_to_fit();
  list.m_NumberLiteral.shrink_to_fit();
  return list;
}

}  // namespace loxt
