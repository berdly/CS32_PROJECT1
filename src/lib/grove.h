#ifndef GROVE_H
#define GROVE_H

#include "parser.h"
#include <vector>
#include <map>
#include <optional>
#include "error.h"
#include <stdexcept>
#include <iostream>
#include <variant>
#include <utility>

typedef std::variant<double, bool, void*> Var;

enum class TreeType{
EXP,
IF,
WHILE,
PRINT,
RETURN,
};

class ASGrove{
  std::vector<ASTree*> statements;
  std::vector<TreeType> types; //A reference Vector to tell what type the trees are
  std::map<std::string, Var> vars;
  std::map<std::string, Func> funcs;
  
  unsigned place; //how many trees have been executed
  const ASGrove* parent;
  bool is_func;
  std::optional<Var> search_var(const std::string& query) const;
  Var calcHelp(const ASTree::ASNode&);
  void printHelp(const ASTree::ASNode&) const;
  
public:
  ASGrove();
  ASGrove(std::vector<std::vector<Token>> commands, unsigned start, unsigned end, ASGrove* owner = nullptr, bool func = false);
  ASGrove(std::vector<std::vector<Token>> commands, ASGrove* owner = nullptr, bool func = false);
  ~ASGrove();
  void update_existing(const std::map<std::string, Var>&); //If a Variable exists both in the lower grove and upper grove, takes the value from the lower grove and assigns it to the upper grove. (Makes i++) 
  const std::map<std::string, Var>& show_vars() const;
  Var eval(); // Evaluates all trees sequentially until the end
  Var calc(bool print = true); //Stepper evaluates one tree one step at a time
  void add_tree(ASTree* tree, TreeType type = TreeType::EXP);
  void reset(); // Will reset the placement of the tree
  void printAll(unsigned indent = 0) const;
  void print(unsigned i, unsigned indent) const;
  void print_curr() const;
  void clear();
  void add_var(const std::string& name, Var val);
};
class StatementTree: public ASTree{
  private:
  ASGrove* body;
  StatementTree* next;
  public:
  StatementTree(const ASTree& conditional, ASGrove* block) : ASTree{conditional}, body{block}, next{nullptr} {}
  virtual ~StatementTree();
  ASGrove* get_body() { return body; }
  StatementTree* get_next() const { return next; }
  void update_down(const std::map<std::string, Var>&);
  void push_back(StatementTree*);
  void clear() {this->body->clear();}
};
class Func{
  private:
  ASGrove body;
  std::vector<std::string> names;
  public:
  Func(const std::vector<Token>& tokens, ASGrove* owner): body{}, names{} {
    unsigned var_end{};
    if(tokens.at(2).get_type() != TokenType::LPAR){
      throw ParserError{tokens.at(2)};
    }
    TokenType last{TokenType::COMMA};
    for(unsigned i{3}; i < tokens.size(); i++){
      switch(tokens.at(i).get_type()){
        case TokenType::RPAR:
           var_end = i;
           break;
        case TokenType::VAR:
          if(last == TokenType::VAR){
            throw ParserError(tokens.at(i));
          }
          names.push_back(tokens.at(i).get_text());
          last = TokenType::VAR;
          break;
        case TokenType::COMMA:
          last = TokenType::COMMA;
          break;
        default:
          throw ParserError(tokens.at(i));
          break;
      }
      if(var_end > 0){
        break;
      }
    }
    if(var_end == 0 || var_end >= tokens.size() - 3){
      throw ParserError(tokens.back());
    }
    if(tokens.at(var_end + 1).get_type() != TokenType::LBRACE){
      throw ParserError(tokens.at(var_end + 1));
    }
    if(tokens.back().get_type() != TokenType::RBRACE){
      throw ParserError(tokens.back());
    }
    body = ASGrove{split_infix(tokens, var_end + 2, static_cast<unsigned>(tokens.size() - 2)), owner};
  }
  std::optional<Var> operator()(const std::vector<Var>& args){
    if(args.size() != names.size()){
      throw ArgError{};
    }
    Func new_scope{*this};
    for(unsigned i{}; i < names.size(); i++){
      new_scope.body.add_var(names.at(i), args.at(i));
    }
    return new_scope.body.eval();
  }
};

#endif
