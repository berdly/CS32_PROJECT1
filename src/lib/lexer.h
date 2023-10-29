#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <iostream>
#include "token.h"

std::vector<Token> reader(const std::string& input, bool err = true);
std::vector<std::vector<Token>> split(const std::vector<Token>& input, unsigned start=0, unsigned end=0);

#endif
