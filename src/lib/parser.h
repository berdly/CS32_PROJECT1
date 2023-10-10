#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "token.h"

class ASTree{

  struct ASnode{
    Token pdata;
    std::vector<ASnode> pchildren;
  }

  ASnode proot;

public:
  ASTree(const std::vector<Token>& tokens);
}
#endif
