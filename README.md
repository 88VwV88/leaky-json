## Leaky - Modern C++ based JSON parser

### Features:

- Recursive Descent Parser
- Tokenization using C++11 regex library
- Token types declared using C++17 `std::variant`
- Parsing using concrete structure types
- Minimal overhead of token parsing
- Write to the standard output stream or a specified file
- Read input from the standard input stream or a specified file
- Complete unicode support
- Complete JSON parsing according to the JSON RFC
- Output using C++23 `std::print` and `std::println`

### Build Dependencies:

- C++23 compatible compiler (gcc-14+ or clang-19+)
- Catch2 testing framework

### Build Instructions:

```{bash}
$> mkdir build && cd build
$> cmake -DCMAKE_BUILD_TYPE=Release ..
$> cmake --build .
```

### Concrete lexer types:

- `token` - base class for token classes
- `string` - JSON string tokens matched using `STRINGS`
- `number` - JSON number tokens matched using `NUMBERS`
- `separator` - JSON allowed character symbols matched using `SEPARATORS`
- `true_token` - JSON `true` symbol token
- `false_token` - JSON `false` symbol token
- `null` - JSON `null` symbol token

### Concrete parser types:

- `json_value` - `std::variant` class representing all valid value types
- `json_array` - class representing the JSON array type
- `array_elements` - collection of `json_value` for JSON values in an array
- `json_object` - class representing the JSON object type
- `json_pair` - class representing key value pairs in a JSON object
- `object_members` - collection of key-value pairs for a JSON object

### Context Free Grammar for JSON LL(1) grammar:

- `<json> ::= <value>`
- `<value> ::= <object> | <array> | "string" | "number" | <true> | <false> | <null>`
- `<object> ::= "{" <members> "}"`
- `<members> ::= <pair> | <pair>, <members> | epsilon`
- `<pair> ::= "string" ":" <value>`
- `<array> ::= "[" <elements> "]`
- `<elements> := <value> | <value>, <elements>`
- `<true> ::= "true"`
- `<false> ::= "false"`
- `<null> ::= "null"`
