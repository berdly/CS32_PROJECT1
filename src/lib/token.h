#ifndef TOKEN_H
#define TOKEN_H

#include <string>
enum class TokenType{
  EXP,
  CONST,
  ERR,
};

class Token{
  int pcolumn;
  int pline;
  std::string ptext;
  TokenType ptype;
public:
  Token(int column, int line, const std::string& text, TokenType type) : pcolumn{column}, pline{line}, ptext{text}, ptype{type} {}
  int get_col() const { return this->pcolumn; };
  int get_line() const { return this->pline; };
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

