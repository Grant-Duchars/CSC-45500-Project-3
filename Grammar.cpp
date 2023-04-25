/**
 * Author: Grant Duchars
 */
#include <fstream>
#include <iostream>
#include <sstream>

#include "Grammar.hpp"

bool Grammar::parse_file() {
  _tok.get(_is);
  return get_statement_list();
}

bool Grammar::get_statement_list() {
  if (!get_statement())
    return true;
  // Check for SEMICOLON
  if (_tok.type() != SEMICOLON)
    return false;
  _code << _tok.value() << " ";
  _tok.get(_is);
  return get_statement_list();
}

bool Grammar::get_statement() {
  // Check for ID
  if (_tok.type() != ID)
    return false;
  _code << _tok.value();
  _tok.get(_is);
  // Check for LPAREN or ASSIGNOP
  if (_tok.type() == LPAREN) {
    _code << _tok.value();
    _tok.get(_is);
  } else if (_tok.type() == ASSIGNOP) {
    _code << " " << _tok.value() << " ";
    _tok.get(_is);
    return get_rhs();
  } else {
    return false;
  }
  // Check for optional ID
  if (_tok.type() == ID) {
    _code << _tok.value();
    _tok.get(_is);
  }
  // Check for RPAREN
  if (_tok.type() != RPAREN)
    return false;
  _code << _tok.value();
  _tok.get(_is);
  return true;
}

bool Grammar::get_rhs() {
  // Check for ID
  if (_tok.type() != ID)
    return false;
  _code << _tok.value();
  _tok.get(_is);
  // Check for LPAREN
  if (_tok.type() != LPAREN)
    return true;
  _code << _tok.value();
  _tok.get(_is);
  // Check for NUM_INT
  if (_tok.type() != NUM_INT)
    return false;
  _code << _tok.value();
  _tok.get(_is);
  // Check for RPAREN
  if (_tok.type() != RPAREN)
    return false;
  _code << _tok.value();
  _tok.get(_is);
  return true;
}