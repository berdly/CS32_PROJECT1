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
  };

private:
  ASNode proot;
  std::vector<Token> infixToPostfix(const std::vector<Token>& tokens);
  int getPrecedence(const Token& token);
  std::vector<Token> postfixToInfix(const std::vector<Token>& postfixTokens);

public:
  
  ASTree(const std::vector<Token>& tokens);
  ASNode build(const std::vector<Token>& tokens, int start, int end);
  std::vector<std::pair<int,int>> get_child_idx(const std::vector<Token>& tokens, int start, int end);
  //double calc();
  //void print();
  const ASNode& getProot() const;
  std::vector<Token> infixWithParens(const std::vector<Token>& tokens);
};

#endif
