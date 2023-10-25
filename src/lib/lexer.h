#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <iostream>
#include "token.h"

std::vector<Token> reader(const std::string& input);
std::vector<std::vector<Token>> split(const std::vector<Token>& input);

#endif
