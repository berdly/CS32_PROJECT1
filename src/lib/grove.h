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
EXP;
IF;
WHILE;
}

class ASGrove{
  std::vector<ASTree*> statements;
  std::vector<TreeType> types;
  std::map<std::string, Var> vars;
  unsigned place; //how many trees have been executed

  void add_var(const std::string& name, Var val);
  std::optional<Var> search_var(const std::string& query);
  virtual Var calcHelp(const ASTree::ASNode&);
  virtual void printHelp(const ASTree::ASNode&) const;
public:
  ASGrove(std::vector<std:::vector<Token>>);
  const std::map<std::string, Var>& show_vars() const;
  Var eval();
  Var calc();
  void add_tree(ASTree*);
  virtual void print() const;
};

#endif
