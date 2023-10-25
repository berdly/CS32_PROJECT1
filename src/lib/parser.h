#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <utility>
#include "token.h"

class ASTree{

  class ASNode{
    Token pdata;
    std::vector<ASNode> pchildren;
    public:
    ASNode();
    ASNode(Token pdata);
    Token get_pdata();

    void add_child(ASNode child);
    double calcHelp();
    void printHelp();
    
    std::vector<ASNode> getkids();
  };

  
  ASNode proot;

public:
  
  ASTree(const std::vector<Token>& tokens);
  ASNode build(const std::vector<Token>& tokens, int start, int end);
  ASNode buildInfix(const std::vector<Token>& tokens);
  std::vector<std::pair<int,int>> get_child_idx(const std::vector<Token>& tokens, int start, int end);
  double calc();
  void print();
  ASNode getProot();
  double precendense(std::string text);

};

#endif
