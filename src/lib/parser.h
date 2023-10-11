#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <utility>
#include "token.h"

class ASTree{

  class ASnode{
    Token pdata;
    std::vector<ASnode> pchildren;
    public:
    ASnode(Token pdata);
    void add_child(ASNode child);
  }

  ASnode proot;

public:
  ASTree(const std::vector<Token>& tokens);
  ASNode build(const std::vector<Token>& tokens, int start, int end);
  std::vector<std::pair<int>> get_child_idx(const std::vector<Token>& tokens, int start, int end);
}
#endif
