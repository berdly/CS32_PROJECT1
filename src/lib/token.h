#ifndef TOKEN_H
#define TOKEN_H

#include <string>
/*
used in switches to determine what kind of value a token is
EXP is an expression/operator, CONST is a constant, LPAR is a left parentheses, RPAR is a right one, and ERR means there was a problem reading
*/
enum class TokenType{
  EXP,
  CONST,
  VAR,
  EQUAL,
  LPAR,
  RPAR,
  ERR,
  IF,
  ELSE,
  WHILE,
  PRINT,
  ASSIGN,
  LOG,
  BOOL,
  LBRACE,
  RBRACE,
  //new
  DEF, //keyword 'def'
  COMMA, // ,
  LBRACK, // [
  RBRACK, // ]
  SEMI, // ;
  FUNC, //for regular functions
  SPECIAL, //for special functions
  //CALL, 
  // for left parentheses which come after variables, indicates function call
  //ACCESS, 
  // for left brackets which come after variables, indicates array access
};
//contains all information needed about a given token, including what kind of value it is through the ptype member, also has accessors
class Token{
  int pcolumn;
  int pline;
  std::string ptext;
  TokenType ptype;
public:
  Token(int column, int line, const std::string& text, TokenType type) : pcolumn{column}, pline{line}, ptext{text}, ptype{type} {}
  Token() : pcolumn{0}, pline{0}, ptext{""}, ptype{TokenType::ERR}{}
  int get_col() const { return this->pcolumn; };
  int get_line() const { return this->pline; };
  void change_line(int line) { pline = line; };
  const std::string& get_text() const { return this->ptext; };
  TokenType get_type() const { return this->ptype; };
};
#endif
/*

how to respond to different token types:

switch(token1.get_type()){
case TokenType::EXP:
  do_something();
  break;
case TokenType::CONST:
  do_something_else();
  break;
default:
  break;
}
*/

