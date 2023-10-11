#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "token.h"

class ASTree{

  class ASnode{
    Token pdata;
    std::vector<ASnode> pchildren;

    void buildHelp(const std::vector<Token>& tokens, int idx);
    
    ASnode(Token pdata);
  }

  ASnode proot;

public:
  ASTree(const std::vector<Token>& tokens);

  
}
#endif
