#include "Lexer.hpp"
#include <exception>
#include <iostream>
#include "EventLogger.hpp"

int main(int argc, char *argv[]) {
  INFO("Web server initialising");
  const char *configfile_path = "./config/default.config";
  if (argc > 2) {
    ERROR("Usage: ./webserv or ./webserv [Path to configfile]");
    return 1;
  }
  if (argc == 2)
    configfile_path = argv[1];
  try {
    Lexer tokens(configfile_path);
    std::cout << tokens << std::endl;
  } catch (std::runtime_error const &e) {
    ERROR(e.what());
    // std::cerr << e.what() << std::endl;
    return 1;
  }
  return 0;
}