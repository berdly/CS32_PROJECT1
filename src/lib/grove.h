#ifndef GROVE_H
#define GROVE_H


#include "parser.h"
#include <vector>
#include <map>
#include <optional>
#include "error.h"
#include <stdexcept>
#include <iostream>

class ASGrove{
  std::vector<ASTree> statements;
  std::map<std::string, double> vars;
  unsigned place; //how many trees have been executed

  void add_var(const std::string& name, double val);
  std::optional<double> search_var(const std::string& query);
  double calcHelp(const ASTree::ASNode&);
  void printHelp(const ASTree::ASNode&) const;
public:
  ASGrove(const std::vector<ASTree>&);
  ASGrove(const ASTree&);
  ASGrove();
  double eval();
  double calc();
  void add_tree(const ASTree&);
  void print() const;
};
#endif




/*
#include <typeinfo>
#include <typeid>
#include <any>
#include <optional>

//for later, exploration into implementing a version of grove which can hold variables of ARBITRARY TYPE:)
//having trouble with building a nice way to revert the data from std::any to the original type without knowing manually entering the original type
//if anyone has any ideas lmk

class VarDict{
  std::map<std::string, std::pair<std::any, std::type_index>> dict;
  
  template<class T>
  void add(const std::string& name, const T& data); 
  std::pair<std::any, std::type_index> access(const std::string& 
}
template<class ReturnType>
class ASGrove{
  static std::set<std::type_index> valid_types;
  std::vector<ASTree> statements;
  VarDict variables;
  
  std::optional<ReturnType> step();
  public:
  ReturnType eval();
  std::map<std::string, double> getVariables();
};
*/
