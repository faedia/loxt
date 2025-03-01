#include <argparse/argparse.hpp>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <istream>
#include <loxt/lexer.hpp>
#include <sstream>
#include <string>

auto run_file(const std::string& path) -> void {
  std::string contents;
  {
    std::ifstream file(path);
    std::stringstream stream;
    stream << file.rdbuf();
    contents = stream.str();
  }
  const auto& toks = loxt::lex(contents);
  // for (auto toks = loxt::lex(contents); auto tok : toks) {
  //   // std::cout << toks.to_string(tok) << std::endl;
  // }
  std::cout << toks.has_error() << '\n';
}

auto run_interpreter() -> void {
  std::string line;
  for (;;) {
    std::cout << "loxt> ";
    std::getline(std::cin, line);
    for (auto toks = loxt::lex(line); auto tok : toks) {
      std::cout << toks.to_string(tok) << std::endl;
    }
  }
}

auto main(int argc, char const* argv[]) -> int {
  argparse::ArgumentParser program(argv[0]);
  program.add_argument("file").nargs(argparse::nargs_pattern::optional);

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(EXIT_FAILURE);
  }

  if (program.present("file").has_value()) {
    run_file(program.get<std::string>("file"));
  } else {
    run_interpreter();
  }

  return 0;
}
