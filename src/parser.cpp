#include "parser.hpp"
#include "lexer.hpp"
#include <memory>

/* json_object parsing logic */

json_object::json_object(container_t &tokens) {
  expect(separator('{'), tokens);
  members = object_members(tokens);
  expect(separator('}'), tokens);
}
object_members::object_members(container_t &tokens) {
  values.emplace_back(tokens);

  auto lookahead = tokens.front();
  while (lookahead == separator(',')) {
    tokens.pop();
    values.emplace_back(tokens);
    lookahead = tokens.front();
  }
}
json_pair::json_pair(container_t &tokens) {
  if (!std::holds_alternative<string>(tokens.front()))
    throw invalid_token_error(string{"[key]"}, tokens.front());

  key = std::get<string>(tokens.front());
  tokens.pop();
  expect(separator(':'), tokens);
  value = json_value(tokens);
}

/* json_array parsing logic */

json_array::json_array(container_t &tokens) {
  expect(separator('['), tokens);
  elements = array_elements(tokens);
  expect(separator(']'), tokens);
}
array_elements::array_elements(container_t &tokens) {
  values.emplace_back(tokens);

  auto lookahead = tokens.front();
  while (lookahead == separator(',')) {
    tokens.pop();
    values.emplace_back(tokens);
    lookahead = tokens.front();
  }
}

/* json value parsing logic */

json_value::json_value(container_t &tokens) {
  auto lookahead = tokens.front();

  if (std::holds_alternative<separator>(lookahead)) {
    switch (static_cast<char>(std::get<separator>(lookahead).type)) {
    case '{':
      val = std::make_unique<json_object>(tokens);
      break;
    case '[':
      val = std::make_unique<json_array>(tokens);
      break;
    default:
      throw invalid_token_error(null{}, std::get<separator>(lookahead));
    }
  } else if (std::holds_alternative<string>(lookahead)) {
    val = std::get<string>(lookahead);
    tokens.pop();
  } else if (std::holds_alternative<number>(lookahead)) {
    val = std::get<number>(lookahead);
    tokens.pop();
  } else if (std::holds_alternative<true_token>(lookahead)) {
    val = std::get<true_token>(lookahead);
    tokens.pop();
  } else if (std::holds_alternative<false_token>(lookahead)) {
    val = std::get<false_token>(lookahead);
    tokens.pop();
  } else if (std::holds_alternative<null>(lookahead)) {
    val = std::get<null>(lookahead);
    tokens.pop();
  } else
    throw invalid_token_error(null{}, null{});
}

auto to_string(json_value const &value, unsigned current_indent)
    -> std::string {
  if (std::holds_alternative<string>(value.val))
    return std::format("\"{}\"", std::get<string>(value.val).value);

  if (std::holds_alternative<number>(value.val))
    return std::get<number>(value.val).value;

  if (std::holds_alternative<std::unique_ptr<json_object>>(value.val))
    return to_string(std::get<std::unique_ptr<json_object>>(value.val).get(),
                     current_indent);

  if (std::holds_alternative<std::unique_ptr<json_array>>(value.val))
    return to_string(std::get<std::unique_ptr<json_array>>(value.val).get(),
                     current_indent);

  if (std::holds_alternative<true_token>(value.val))
    return "true";

  if (std::holds_alternative<false_token>(value.val))
    return "false";

  if (std::holds_alternative<null>(value.val))
    return "null";

  return "<unknown>";
}

auto to_string(json_object const *object, unsigned current_indent)
    -> std::string {
  if (object->members.values.size() == 0)
    return "{}";

  std::stringstream out;
  out << "{\n";
  ++current_indent;
  for (auto i = 0; i < object->members.values.size() - 1; ++i) {
    auto const &pair = object->members.values[i];
    out << std::string(current_indent, '\t') << to_string(pair, current_indent)
        << ",\n";
  }
  out << std::string(current_indent, '\t')
      << to_string(object->members.values.back(), current_indent) << '\n';
  out << std::string(--current_indent, '\t') << '}';
  return out.str();
}

auto to_string(json_pair const &pair, unsigned current_indent) -> std::string {
  return std::format("\"{}\": {}", pair.key.value,
                     to_string(pair.value, current_indent));
}

auto to_string(json_array const *array, unsigned current_indent)
    -> std::string {
  if (array->elements.values.size() == 0)
    return "{}";

  std::stringstream out;
  out << "[\n";
  ++current_indent;
  for (auto i = 0; i < array->elements.values.size() - 1; ++i) {
    auto const &element = array->elements.values[i];
    out << std::string(current_indent, '\t')
        << to_string(element, current_indent) << ",\n";
  }
  out << std::string(current_indent, '\t')
      << to_string(array->elements.values.back(), current_indent) << '\n';
  out << std::string(--current_indent, '\t') << ']';
  return out.str();
}