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
  ASNode build(const std::vector<Token>& tokens, int start, int end);
  std::vector<std::pair<int,int>> get_child_idx(const std::vector<Token>& tokens, int start, int end);
  ASTree::ASNode buildInfix(const std::vector<Token>&, unsigned, unsigned, bool);
  int precedence(const std::string& text);


public:
  ASTree(const std::vector<Token>& tokens, unsigned start, unsigned end, bool infix = true);
  ASTree(const std::vector<Token>& tokens, bool infix = true);
  const ASNode& getProot() const;
};


#endif
