#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer.hpp"

#include <format>
#include <memory>
#include <print>
#include <stack>
#include <stdexcept>

template <typename Expected, typename Actual>
struct invalid_token_error : std::runtime_error {
  inline invalid_token_error(Expected expected, Actual actual)
      : std::runtime_error(std::format("expected: {}, actual: {}",
                                       to_string(expected),
                                       to_string(actual))) {}
};

template <typename Expected>
constexpr auto expect(Expected &&expected, container_t &tokens) {
  auto actual = tokens.front();
  if (actual != expected) {
    throw invalid_token_error(expected, actual);
  }
  tokens.pop();
}

struct json_array;
struct json_object;

struct json_value {
  std::variant<std::monostate, std::unique_ptr<json_object>,
               std::unique_ptr<json_array>, string, number, true_token,
               false_token, null>
      val;
  json_value() = default;
  json_value(container_t &tokens);
};

struct json_pair {
  string key = std::string{};
  json_value value;
  json_pair(container_t &tokens);
};
struct object_members {
  std::vector<json_pair> values;
  object_members() = default;
  object_members(container_t &tokens);
};

struct json_object {
  object_members members;
  json_object(container_t &tokens);
};

struct array_elements {
  std::vector<json_value> values;
  array_elements() = default;
  array_elements(container_t &tokens);
};

struct json_array {
  array_elements elements;
  json_array(container_t &tokens);
};

struct json {
  json_value val;
  json(container_t &tokens) : val(tokens) {}
};

template <class T>
inline constexpr auto to_string(T value)
  requires requires(T value) {
    { std::to_string(value) } -> std::same_as<std::string>;
  }
{
  return std::to_string(value);
}
auto to_string(json_array const *array, unsigned current_index = 0)
    -> std::string;
auto to_string(json_value const &value, unsigned current_index = 0)
    -> std::string;
auto to_string(json_pair const &pair, unsigned current_index = 0)
    -> std::string;
auto to_string(json_object const *object, unsigned current_index = 0)
    -> std::string;
// auto to_string(json &&parsed) -> std::string;

#endif