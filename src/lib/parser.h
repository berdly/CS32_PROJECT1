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
    static std::vector<Token> infix_tokens(const std::vector<Token>& tokens); // Infix Function
    static int get_operator_precedence(TokenType type);  // Infix Function Helper for Precedence
  };

private:
  ASNode proot;
  ASNode build(const std::vector<Token>& tokens, int start, int end);
  std::vector<std::pair<int,int>> get_child_idx(const std::vector<Token>& tokens, unsigned start, unsigned end);
  ASTree::ASNode buildInfix(const std::vector<Token>&, int, int);
  double precedence(std::string text);

public:
  ASTree(const std::vector<Token>& tokens, bool infix = false);
  const ASNode& getProot() const;
};

#endif
