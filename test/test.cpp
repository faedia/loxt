#include <gtest/gtest.h>

#include "loxt/lexer.hpp"

TEST(HelloTest, BasicAssertions) {
  EXPECT_EQ(loxt::TokenKind::And(), loxt::TokenKind::And());
  EXPECT_EQ(loxt::TokenKind::And(), loxt::TokenKind::If());
}
