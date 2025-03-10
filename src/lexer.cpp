#include <cctype>
#include <format>
#include <loxt/lexer.hpp>
#include <print>

namespace loxt {

namespace {
auto report(SourceLocation loc, const std::string& error_text) -> void {
  std::println("{}:{}: Error: {}\n", loc.line, loc.column, error_text);
}

const std::string TokenNames[] = {
#define LOXT_TOKEN(name) #name,
#include <loxt/token_kinds.def>
};

const std::unordered_map<std::string_view, const TokenKind> Keywords = {
#define LOXT_KEYWORD_TOKEN(name, spelling) {spelling, TokenKind::name()},
#include <loxt/token_kinds.def>
};
}  // namespace

auto SourceLocation::operator++() -> SourceLocation& {
  if (*pos++ == '\n') {
    column = 1;
    ++line;
  } else {
    ++column;
  }
  return *this;
}

auto SourceLocation::operator++(int) -> SourceLocation {
  auto old = *this;
  this->operator++();
  return old;
}

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

auto lex(const std::string& source) -> std::shared_ptr<TokenList> {
  auto list = std::shared_ptr<TokenList>(new TokenList(source));

  SourceLocation loc{.line = 1, .column = 1, .pos = source.begin()};

  while (loc.pos != source.end()) {
    SourceLocation start_loc = loc;
    char chr = *(loc++).pos;
    switch (chr) {
      case '(':
        list->m_Tokens.emplace_back(TokenKind::LeftParen(), start_loc, 0);
        break;
      case ')':
        list->m_Tokens.emplace_back(TokenKind::RightParen(), start_loc, 0);
        break;
      case '{':
        list->m_Tokens.emplace_back(TokenKind::LeftBrace(), start_loc, 0);
        break;
      case '}':
        list->m_Tokens.emplace_back(TokenKind::RightBrace(), start_loc, 0);
        break;
      case ',':
        list->m_Tokens.emplace_back(TokenKind::Comma(), start_loc, 0);
        break;
      case '.':
        list->m_Tokens.emplace_back(TokenKind::Period(), start_loc, 0);
        break;
      case '-':
        list->m_Tokens.emplace_back(TokenKind::Minus(), start_loc, 0);
        break;
      case '+':
        list->m_Tokens.emplace_back(TokenKind::Plus(), start_loc, 0);
        break;
      case ';':
        list->m_Tokens.emplace_back(TokenKind::SemiColon(), start_loc, 0);
        break;
      case '*':
        list->m_Tokens.emplace_back(TokenKind::Asterisk(), start_loc, 0);
        break;
      case '!':
        list->m_Tokens.emplace_back(match('=', source, loc)
                                        ? TokenKind::BangEqual()
                                        : TokenKind::Bang(),
                                    start_loc, 0);
        break;
      case '=':
        list->m_Tokens.emplace_back(match('=', source, loc)
                                        ? TokenKind::EqualEqual()
                                        : TokenKind::Equal(),
                                    start_loc, 0);
        break;
      case '<':
        list->m_Tokens.emplace_back(match('=', source, loc)
                                        ? TokenKind::LessEqual()
                                        : TokenKind::Less(),
                                    start_loc, 0);
        break;
      case '>':
        list->m_Tokens.emplace_back(match('=', source, loc)
                                        ? TokenKind::GreaterEqual()
                                        : TokenKind::Greater(),
                                    start_loc, 0);
        break;
      case '/':
        if (match('/', source, loc)) {
          while (loc.pos != source.end() && *loc.pos != '\n') {
            ++loc;
          }
        } else {
          list->m_Tokens.emplace_back(TokenKind::BackSlash(), start_loc, 0);
        }
        break;
      case '"':
        while (loc.pos != source.end() && *loc.pos != '"') {
          ++loc;
        }

        if (loc.pos == source.end()) {
          list->m_Tokens.emplace_back(TokenKind::Error(), start_loc, 0);
          report(start_loc, "String is unterminated");
          list->m_HasError = true;
        } else {
          ++loc;
          list->m_Tokens.emplace_back(
              TokenKind::String(), start_loc,
              static_cast<Literal>(list->m_StringLiteral.size()));
          list->m_StringLiteral.emplace_back(start_loc.pos + 1, loc.pos - 1);
        }
        break;
      default:
        if (static_cast<bool>(std::isdigit(chr))) {
          while (loc.pos != source.end() &&
                 static_cast<bool>(std::isdigit(*loc.pos))) {
            ++loc;
          }

          list->m_Tokens.emplace_back(
              TokenKind::Number(), start_loc,
              static_cast<Literal>(list->m_NumberLiteral.size()));
          list->m_NumberLiteral.emplace_back(
              std::stoull(std::string(start_loc.pos, loc.pos)));
        } else if (static_cast<bool>(std::isalpha(chr))) {
          while (loc.pos != source.end() &&
                 static_cast<bool>(std::isalnum(*loc.pos))) {
            ++loc;
          }

          std::string_view identifier_str{start_loc.pos, loc.pos};
          auto keyword_iter = Keywords.find(identifier_str);
          if (keyword_iter == Keywords.end()) {
            auto identifier_iter = list->m_IdentifierMap.find(identifier_str);
            Identifier identifier = list->m_Identifiers.size();
            if (identifier_iter == list->m_IdentifierMap.end()) {
              list->m_IdentifierMap.emplace(identifier_str, identifier);
              list->m_Identifiers.emplace_back(identifier_str);
            } else {
              identifier = identifier_iter->second;
            }
            list->m_Tokens.emplace_back(TokenKind::Identifier(), start_loc,
                                        identifier);
          } else {
            list->m_Tokens.emplace_back(keyword_iter->second, start_loc, 0);
          }
        } else if (std::isspace(chr) == 0) {
          list->m_Tokens.emplace_back(TokenKind::Error(), start_loc, 0);
          report(start_loc, std::format("Unrecognized character '{}'", chr));
          list->m_HasError = true;
        }
    }
  }
  list->m_Tokens.emplace_back(TokenKind::Eof(), loc, 0);
  return list;
}

}  // namespace loxt
