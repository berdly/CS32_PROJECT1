#include "parser.h"
#include <vector>
#include <map>
#include <optional>

class ASGrove{
  std::vector<ASTree> statements;
  std::map<std::string, double> vars;
  void step();
  public:
    void step(); 
    double eval();
    std::map<std::string, double> getVariables(){
        return variables;
    }
    void addVar(const std::string& name, double val){ //should be called anytime an equal sign operator is reached in the eval phase, no need for passing around chunky nodes, we'll extract the data inside eval
                                                              //plus this would require passing in two tokens, a var and a const but a var can be assigned from another var so better to make it more general i think
      variables.emplace(name, val); //emplace is easier and better, makes pair automatically inside dict with no copying
    }
    std::optional<double> searchVar(const std::string& query){ //definitely shouldn't be in tree, trees are statements so they only evaluate, theres only continuity between trees in the same grove(scope)
                                                               //made optional so that it may or may not have a value, can use std::optional<T>::has_value() method to check
      auto value{variables.find(query)};//map iterator type
      return (value == variables.end()) ? {} : std::optional<double>{value->second}; //returns empty option if not found, option with value otherwise
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
