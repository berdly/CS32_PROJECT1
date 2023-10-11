#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <stack>
#include "Token.h"

std::stack<Token> reader(const std::string& input);


#endif
