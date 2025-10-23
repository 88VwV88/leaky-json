#include "lexer.hpp"
#include "parser.hpp"

#include <print>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

using std::operator""sv;

bool check_match(std::string text, std::regex const &re) {
  std::smatch match;
  return std::regex_search(text, match, re);
}

TEST_CASE("[lexer] Test lstrip contexpr utility") {
  REQUIRE(lstrip("    abc   "sv) == "abc   "sv);
  REQUIRE(lstrip("    abc def"sv) == "abc def"sv);
}

TEST_CASE("[lexer] Test STRINGS regex") {
  REQUIRE(check_match("\"hello\"  ", STRINGS));
  REQUIRE(check_match("\"😁\"", STRINGS));
  REQUIRE_FALSE(check_match("  \"hi\"  ", STRINGS));
}

TEST_CASE("[lexer] Test NUMBERS regex") {
  REQUIRE(check_match("12341  ", NUMBERS));
  REQUIRE(check_match("123.456E12 ", NUMBERS));
  REQUIRE(check_match("123e123", NUMBERS));
  REQUIRE_FALSE(check_match("  123", NUMBERS));
}

TEST_CASE("[lexer] Test SEPARATORS regex") {
  REQUIRE(check_match(":  ", SEPARATORS));
  REQUIRE(check_match(",  ", SEPARATORS));
  REQUIRE(check_match("{  ", SEPARATORS));
  REQUIRE(check_match("(  ", SEPARATORS));
  REQUIRE(check_match("[  ", SEPARATORS));
  REQUIRE(check_match("]  ", SEPARATORS));
  REQUIRE(check_match(")  ", SEPARATORS));
  REQUIRE(check_match("}  ", SEPARATORS));
  REQUIRE_FALSE(check_match(R"(  {  "hello": "world  })", SEPARATORS));
}

TEST_CASE("[lexer] Test to_string") {
  REQUIRE(to_string(string{"hello"}) == std::string("<string(\"hello\")>"));
  REQUIRE(to_string(number{"123.456"}) == std::string("<number(123.456)>"));
  REQUIRE(to_string(separator(';')) == std::string("<separator(;)>"));
}

TEST_CASE("[parser] Test parsing json primitives") {
  container_t tokens;

  tokens.push(string{"hello"});
  REQUIRE(string{"hello"} == std::get<string>(json_value{tokens}.val));
  tokens.push(number{"123"});
  REQUIRE(number{"123"} == std::get<number>(json_value{tokens}.val));

  tokens.push(true_token{});
  REQUIRE(std::holds_alternative<true_token>(json_value{tokens}.val));
  tokens.push(false_token{});
  REQUIRE(std::holds_alternative<false_token>(json_value{tokens}.val));
  tokens.push(null{});
  REQUIRE(std::holds_alternative<null>(json_value{tokens}.val));
}