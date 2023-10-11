#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include "token.h"

std::vector<Token> reader(const std::string& input);

#endif
