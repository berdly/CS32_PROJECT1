#ifndef TOKEN_H
#define TOKEN_H
#include <string>
enum class TokenType{
  EXP,
  CONST,
};

class Token{
int column;
int line;
std::string text;
TokenType type;

int get_col() const { return this->column; };
int get_line() const { return this->line; };
const std::string& get_text() const { return this->text; };
TokenType get_type() const { return this->text; };
};
#endif
