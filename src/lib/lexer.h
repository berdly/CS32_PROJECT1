#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <iostream>
#include "token.h"

std::vector<Token> reader(const std::string& input, int l);

#endif
