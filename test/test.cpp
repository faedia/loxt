#include <gtest/gtest.h>

#include "loxt/lexer.hpp"

TEST(LexerTest, LexerTest) {
  std::string str = "int main";
  auto toks = loxt::lex(str);
  EXPECT_EQ(toks.size(), 3);

  auto tok = toks.begin();
  EXPECT_EQ(tok->kind, loxt::TokenKind::Identifier());
  EXPECT_EQ(toks.identifier(tok->identifier), "int");
  EXPECT_EQ(tok->loc.column, 1);
  EXPECT_EQ(tok->loc.line, 1);

  ++tok;
  EXPECT_EQ(tok->kind, loxt::TokenKind::Identifier());
  EXPECT_EQ(toks.identifier(tok->identifier), "main");
  EXPECT_EQ(tok->loc.column, 5);
  EXPECT_EQ(tok->loc.line, 1);

  ++tok;
  EXPECT_EQ(tok->kind, loxt::TokenKind::Eof());
  EXPECT_EQ(tok->loc.column, 9);
  EXPECT_EQ(tok->loc.line, 1);
}
