#ifndef LEXER_HPP
#define LEXER_HPP

#include <queue>
#include <regex>
#include <string_view>
#include <variant>

constexpr auto lstrip(std::string_view string) -> std::string_view {
  size_t start = string.find_first_not_of(" \t\n");
  return {string.begin() + start, string.end()};
}

enum class token_type {
  FALSE,
  TRUE,
  NUMBER,
  STRING,
  NULL_TOKEN,
  COMMA = ',',
  COLON = ':',
  LPARAN = '(',
  RPARAN = ')',
  LBRACE = '{',
  RBRACE = '}',
  LBRACKET = '[',
  RBRACKET = ']',
};

const std::regex SEPARATORS{R"re(^[\[\]\{\}\,\:])re"};
const std::regex KEYWORDS{R"re(^(true|false|null)\b)re"};
const std::regex NUMBERS{
    R"re(^-?(0|[1-9][0-9]*)(\.[0-9]+)?([eE][+\-]?[0-9]+)?)re"};
const std::regex STRINGS{
    R"re(^"(([^"\\\x00-\x1F]|\\["\\\/bfnrt]|\\u[0-9a-fA-F]{4})*)")re"};

struct token {
  token_type type;
};
struct string : token {
  std::string value;
  string(std::string const &value) : token(token_type::STRING), value(value) {}
};
struct number : token {
  std::string value;
  number(std::string const &value) : token(token_type::NUMBER), value(value) {}
};
struct separator : token {
  separator(char ch) { this->type = token_type(ch); }
};
struct true_token : token {
  token_type type = token_type::TRUE;
};
struct false_token : token {
  token_type type = token_type::FALSE;
};
struct null : token {
  token_type type = token_type::NULL_TOKEN;
};

using token_v =
    std::variant<string, number, separator, true_token, false_token, null>;
using container_t = std::queue<token_v>;

inline constexpr auto to_string(token_v const &self) -> std::string {
  if (std::holds_alternative<string>(self))
    return std::format("<string(\"{}\")>", std::get<string>(self).value);
  else if (std::holds_alternative<number>(self))
    return std::format("<number({})>", std::get<number>(self).value);
  else if (std::holds_alternative<true_token>(self))
    return "<true>";
  else if (std::holds_alternative<false_token>(self))
    return "<false>";
  else if (std::holds_alternative<null>(self))
    return "<null>";
  else if (std::holds_alternative<separator>(self))
    return std::format("<separator({})>",
                       static_cast<char>(std::get<separator>(self).type));
  return "<unknown>";
}

inline constexpr auto tokenize(std::string &contents) -> container_t {
  std::smatch match;
  container_t tokens;

  while (!contents.empty()) {
    contents = lstrip(contents);
    if (std::regex_search(contents, match, SEPARATORS)) {
      tokens.push(separator{match.str(0).front()});
    } else if (std::regex_search(contents, match, KEYWORDS)) {
      if (match.str(0) == "true")
        tokens.push(true_token{});
      else if (match.str(0) == "false")
        tokens.push(false_token{});
      else if (match.str(0) == "null")
        tokens.push(null{});
    } else if (std::regex_search(contents, match, NUMBERS)) {
      tokens.push(number{match.str(0).c_str()});
    } else if (std::regex_search(contents, match, STRINGS)) {
      tokens.push(string{match.str(1)});
    } else
      throw std::runtime_error(std::format("unknown symbol at: {}", contents));
    contents.erase(contents.begin(), contents.begin() + match.length(0));
  }
  return tokens;
}

/* operator==(token_v &&, ...) overloads */

constexpr bool operator==(const string &lhs, const string &rhs) {
  return lhs.value == rhs.value;
}
constexpr bool operator==(const token_v &lhs, const string &rhs) {
  return std::holds_alternative<string>(lhs) &&
         std::get<string>(lhs).value == rhs.value;
}
constexpr bool operator==(const number &lhs, const number &rhs) {
  return lhs.value == rhs.value;
}
constexpr bool operator==(const token_v &lhs, const number &rhs) {
  return std::holds_alternative<number>(lhs) &&
         std::get<number>(lhs).value == rhs.value;
}
constexpr bool operator==(const token_v &lhs, const separator &rhs) {
  return std::holds_alternative<separator>(lhs) &&
         std::get<separator>(lhs).type == rhs.type;
}
constexpr bool operator==(const token_v &lhs, const true_token &rhs) {
  (void)rhs;
  return std::holds_alternative<true_token>(lhs);
}
constexpr bool operator==(const token_v &lhs, const false_token &rhs) {
  (void)rhs;
  return std::holds_alternative<false_token>(lhs);
}
constexpr bool operator==(const token_v &lhs, const null &rhs) {
  (void)rhs;
  return std::holds_alternative<null>(lhs);
}

#endif
