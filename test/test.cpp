#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <loxt/lib.hpp>

TEST_CASE("Test", "test") {
  auto res = foo();

  REQUIRE(res == false);
}
