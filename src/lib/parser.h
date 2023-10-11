#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <stack>
#include "Token.h"

std::stack<Token> reader(const std::string& input);

#endif
