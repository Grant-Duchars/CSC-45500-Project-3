/*******************************************
 * File: Token.cpp                         *
 * Author: S. Blythe, Grant Duchars        *
 * Date: 1/26/2023                         *
 * PURPOSE: implementation for Token       *
 *******************************************/

#include "Token.hpp"

#include <fstream>
#include <iomanip>

using namespace std;

void build_DFA();
void skip_whitespace(istream &is);

const int _NUM_STATES = 20;
const int _NUM_CHARS = 256;
char _BUFFER[512];

// the promised global for string equivalents of TokenType enumeration
string TokStr[] = {
    "ERROR", "EOF_TOK",  "NUM_INT", "NUM_REAL", "ADDOP",     "MULOP",  "ID",
    "RELOP", "ASSIGNOP", "LPAREN",  "RPAREN",   "SEMICOLON", "LBRACK", "RBRACK",
    "COMMA", "AND",      "OR",      "INTEGER",  "FLOAT",     "WHILE",  "IF",
    "THEN",  "ELSE",     "VOID",    "BEGIN",    "END"};

// This is a "list" of the keywords. Note that they are in the same order
//   as found on the TokenType enumaration.
static string reserved[] = {"int",  "float", "while", "if", "then",
                            "else", "void",  "begin", "end"};

int **DFA = nullptr;
TokenType *state_to_type = nullptr;

/******************************************************
 *  just prints out the info describing this Token    *
 *    to specified stream                             *
 *                                                    *
 *   os  - the stream to add the Token to             *
 *                                                    *
 *   returns: the updated stream                      *
 ******************************************************/
ostream &Token::print(ostream &os) const {
  os << "{ Type:" << left << setw(10) << TokStr[_type] << " Value:" << left
     << setw(10) << _value << " Line Number:" << _line_num << " }";
  return os;
}

/******************************************************
 *  Fills in information about this Token by reading  *
 *    it from specified input stream                  *
 *                                                    *
 *   is  - the stream to read the Token from          *
 *                                                    *
 *   returns: nothing                                 *
 *                                                    *
 *     **** YOU MUST CODE THIS !!!!!! ****            *
 ******************************************************/
void Token::get(istream &is) {
  // Check for and build DFA if needed
  if (DFA == nullptr) {
    build_DFA();
    _line_num = 1;
  }

  // Reset value of token
  _value = "";

  // Skip to first character on the current line
  skip_whitespace(is);
  if (!is) {
    _type = EOF_TOK;
    return;
  }

  // Process new lines and comments
  char curr_char = is.get();
  while (curr_char == '\n' || curr_char == '#') {
    _line_num++;
    // If it is a comment then consume the whole line
    if (curr_char == '#')
      is.getline(_BUFFER, 512);
    skip_whitespace(is);
    if (!is) {
      _type = EOF_TOK;
      return;
    }
    curr_char = is.get();
  }
  is.putback(curr_char);

  // Start processing characters
  int curr_state = 0;
  int prev_state = -1;
  while (curr_state != -1) {
    curr_char = is.get();
    prev_state = curr_state;
    curr_state = DFA[curr_state][(int)curr_char];
    if (curr_state != -1)
      _value += curr_char;
  }
  // Last character read is not part of token due to current state so put it
  // back
  is.putback(curr_char);

  // Check for reserved
  if (prev_state == 1) {
    for (int i = 0; i < 9; i++) {
      if (reserved[i] == _value) {
        _type = static_cast<TokenType>(INTEGER + i);
        return;
      }
    }
  }
  _type = state_to_type[prev_state];
}

/**
 * Skips any whitespace and puts back the first character comsumed.
 */
void skip_whitespace(istream &is) {
  char c;
  c = is.get();
  while (c == ' ' || c == '\t')
    c = is.get();
  is.putback(c);
}

void build_DFA() {
  // Build DFA
  DFA = new int *[_NUM_STATES];
  for (int state = 0; state < _NUM_STATES; state++) {
    DFA[state] = new int[_NUM_CHARS];
  }
  fill_n(&DFA[0][0], _NUM_STATES * _NUM_CHARS, -1);
  // State 0
  fill(&DFA[0][(int)'A'], &DFA[0][(int)'Z'], 1);
  fill(&DFA[0][(int)'a'], &DFA[0][(int)'z'], 1);
  fill(&DFA[0][(int)'0'], &DFA[0][(int)'9'], 2);
  DFA[0][(int)'+'] = DFA[0][(int)'-'] = 5;
  DFA[0][(int)'*'] = DFA[0][(int)'/'] = 6;
  DFA[0][(int)'<'] = DFA[0][(int)'>'] = 7;
  DFA[0][(int)'='] = 9;
  DFA[0][(int)'('] = 10;
  DFA[0][(int)')'] = 11;
  DFA[0][(int)'&'] = 12;
  DFA[0][(int)'|'] = 14;
  DFA[0][(int)';'] = 16;
  DFA[0][(int)'['] = 17;
  DFA[0][(int)']'] = 18;
  DFA[0][(int)','] = 19;
  // State 1
  fill(&DFA[1][(int)'A'], &DFA[1][(int)'Z'], 1);
  fill(&DFA[1][(int)'a'], &DFA[1][(int)'z'], 1);
  fill(&DFA[1][(int)'0'], &DFA[1][(int)'9'], 1);
  // State 2
  fill(&DFA[2][(int)'0'], &DFA[2][(int)'9'], 2);
  DFA[2][(int)'.'] = 3;
  // State 3
  fill(&DFA[3][(int)'0'], &DFA[3][(int)'9'], 4);
  // State 4
  fill(&DFA[4][(int)'0'], &DFA[4][(int)'9'], 4);
  // State 7
  DFA[7][(int)'='] = 8;
  // State 9
  DFA[9][(int)'='] = 8;
  // State 12
  DFA[12][(int)'&'] = 13;
  // State 14
  DFA[14][(int)'|'] = 15;

  // Build state_to_type
  state_to_type = new TokenType[20];
  state_to_type[0] = ERROR;
  state_to_type[1] = ID;
  state_to_type[2] = NUM_INT;
  state_to_type[3] = ERROR;
  state_to_type[4] = NUM_REAL;
  state_to_type[5] = ADDOP;
  state_to_type[6] = MULOP;
  state_to_type[7] = RELOP;
  state_to_type[8] = RELOP;
  state_to_type[9] = ASSIGNOP;
  state_to_type[10] = LPAREN;
  state_to_type[11] = RPAREN;
  state_to_type[12] = ERROR;
  state_to_type[13] = AND;
  state_to_type[14] = ERROR;
  state_to_type[15] = OR;
  state_to_type[16] = SEMICOLON;
  state_to_type[17] = LBRACK;
  state_to_type[18] = RBRACK;
  state_to_type[19] = COMMA;
}
