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
typedef std::variant<double, bool> Var;

enum class TreeType{
EXP,
IF,
WHILE,
PRINT,
};

class ASGrove{
  std::vector<ASTree*> statements;
  std::vector<TreeType> types;
  std::map<std::string, Var> vars;
  unsigned place; //how many trees have been executed
  const ASGrove* parent;

  void add_var(const std::string& name, Var val);
  std::optional<Var> search_var(const std::string& query) const;
  Var calcHelp(const ASTree::ASNode&);
  void printHelp(const ASTree::ASNode&) const;
  
public:
  ASGrove();
  ASGrove(std::vector<std::vector<Token>> commands, unsigned start, unsigned end, ASGrove* owner = nullptr);
  ASGrove(std::vector<std::vector<Token>> commands, ASGrove* owner = nullptr);
  ~ASGrove();
  void update_existing(const std::map<std::string, Var>&);
  const std::map<std::string, Var>& show_vars() const;
  Var eval();
  Var calc(bool print = true);
  void add_tree(ASTree* tree, TreeType type = TreeType::EXP);
  void reset();
  void print() const;
};
class StatementTree: public ASTree{
  private:
  ASGrove body;
  StatementTree* next;
  public:
  StatementTree(const ASTree& conditional, const ASGrove& block) : ASTree{conditional}, body{block} {}
  ASGrove& get_body() { return body; }
  const StatementTree* get_next() const { return next; }
};

#endif
