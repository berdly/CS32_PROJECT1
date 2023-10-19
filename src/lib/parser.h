#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <utility>
#include "token.h"

class ASTree{
public:
  class ASNode{
    Token pdata;
    std::vector<ASNode> pchildren;
    public:
    ASNode();
    ASNode(Token pdata);
    const Token& get_pdata() const;

    void add_child(ASNode child);
    //double calcHelp();
    //void printHelp() const;

   //tree shouldn't have variable val
    
    const std::vector<ASNode>& get_kids() const;
    static std::vector<Token> infix(const std::vector<Token>& tokens); // Infix Function
  };

private:
  ASNode proot;

public:
  
  ASTree(const std::vector<Token>& tokens);
  ASNode build(const std::vector<Token>& tokens, int start, int end);
  std::vector<std::pair<int,int>> get_child_idx(const std::vector<Token>& tokens, int start, int end);
  //double calc();
  //void print();
  ASNode getProot();
};

#endif
