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
  std::cout << contents;
}

auto run_interpreter() -> void {
  std::string line;
  for (;;) {
    std::cout << "loxt> ";
    std::getline(std::cin, line);
    std::cout << line;
  }
}

auto main(int argc, char const* argv[]) -> int {
  std::string prog = "hello world";
  loxt::TokenList list = loxt::lex(prog);
  for (auto& tok : list) {
    std::cout << tok.kind.name() << " " << tok.loc.line << " " << tok.loc.column
              << " " << list.identifier(tok.identifier) << " " << list.source()
              << std::endl;
  }

  {
    const auto& tok = *list.begin();
    std::cout << tok.kind.name() << " " << tok.loc.line << " " << tok.loc.column
              << " " << list.identifier(tok.identifier) << " " << list.source()
              << std::endl;
  }

  {
    const auto& tok = *(list.begin() + 1);
    std::cout << tok.kind.name() << " " << tok.loc.line << " " << tok.loc.column
              << " " << list.identifier(tok.identifier) << " " << list.source()
              << std::endl;
  }

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
