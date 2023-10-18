#include "parser.h"
#include <vector>
#include <map>

template <class T> class ASGrove{
    std::vector<ASTree> statements;
    std::map<std::string, double> variables;
    

  public:
    void step();
    T eval();
    std::map<std::string, double> getVariables(){
        return variables;
    }
    /* void addVariable(ASTree::ASNode add, double val){ // needs to be figured out - unknown what grove the node shoudl belong to when this method is called....
      string a = add.get_pdata().get_text();            //might need to be in the tree class... calc will be called by trees
      double d = stod(add.get_pdata().get_text());
      
      variables.insert(std::pair<std::string, double>(a, d));

    } */
    void searchVariable(Token find){ // probably needs to be in tree class too, variables might need to be stored in trees, and combined later
      return variables[find.get_text()];
    }

};

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
class ASGrove<template class ReturnType>{
  static std::set<std::type_index> valid_types;
  std::vector<ASTree> statements;
  VarDict variables;
  
  std::optional<ReturnType> step();
  public:
  ReturnType eval();
  std::map<std::string, double> getVariables();
};
*/
