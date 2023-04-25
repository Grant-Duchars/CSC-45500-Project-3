/**
 * Author: Grant Duchars
 */
#ifndef _GRAMMAR_HPP_
#define _GRAMMAR_HPP_

#include <fstream>
#include <sstream>
#include <string>

#include "Token.hpp"

class Grammar {
private:
  // Private Variables
  Token _tok;
  std::stringstream _code;
  std::istream &_is;

  // Private Methods
  bool get_statement_list();
  bool get_statement();
  bool get_rhs();

public:
  // Constructor
  Grammar(std::istream &is) : _tok(), _code(), _is(is) {}

  // Getters
  std::string code() { return _code.str(); }

  // Public Methods
  bool parse_file();
};
#endif