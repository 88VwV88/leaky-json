#include <fstream>
#include <iostream>
#include <print>
#include <sstream>
#include <unistd.h>

#include "lexer.hpp"
#include "parser.hpp"

auto display_help_message(char *argv[]) -> void {
  std::println("Usage: {} [OPTION]... [FILE]...\nOptions:", argv[0]);
  for (auto &[flag, description] : {
           std::pair{'h', "display this help message"},
           std::pair{'i', "input file name"},
           std::pair{'o', "output file name"},
       })
    std::println("  -{}\n    {}", flag, description);
}

auto read_input(std::istream &in = std::cin) -> std::string {
  std::string line;
  std::stringstream stream;

  do {
    std::getline(std::cin, line);
    stream << line;
  } while (!std::cin.eof());

  return stream.str();
}

int main(int argc, char *argv[]) {
  char opt;
  std::string input_file, output_file;

  while ((opt = getopt(argc, argv, "hi:o:")) != -1) {
    switch (opt) {
    case 'i':
      input_file = optarg;
      break;
    case 'o':
      output_file = optarg;
      break;
    case 'h':
      display_help_message(argv);
      return 0;
    default:
      display_help_message(argv);
      return 1;
    }
  }

  std::string contents;
  if (input_file.empty())
    contents = read_input();
  else {
    std::ifstream in{input_file};
    if (in.is_open())
      contents = read_input(in);
    else
      throw std::runtime_error("failed to read " + output_file);
  }

  auto tokens = tokenize(contents);
  json parsed{tokens};

  if (output_file.empty())
    std::println(std::cout, "{}", to_string(parsed.val));
  else {
    std::ofstream out{output_file};
    if (out.is_open())
      std::println(out, "{}", to_string(parsed.val));
    else
      throw std::runtime_error("failed to write to " + output_file);
  }

  return 0;
}