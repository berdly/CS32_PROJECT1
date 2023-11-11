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
#include <memory>

class Var;
typedef std::vector<Var>* Arr;

class Var{
  std::variant<double, bool, Arr> data;
  public:
  Var() = default;
  Var(double d): data{d} {}
  Var(bool b): data{b} {}
  Var(Arr a): data{a} {}
  bool holds_bool() const{
    return(std::holds_alternative<bool>(this->data));
  }
  bool holds_Arr() const{
    return(std::holds_alternative<Arr>(this->data));
  }
  bool holds_double() const{
    return(std::holds_alternative<double>(this->data));
  }
  double get_double() const{
    return(std::get<double>(this->data));
  }
  bool get_bool() const{
    return(std::get<bool>(this->data));
  }
  Arr get_Arr() const{
    return(std::get<Arr>(this->data));
  }
};
namespace Specials{
std::optional<Var> pop(const std::vector<Var>& args);
std::optional<Var> len(const std::vector<Var>& args);
std::optional<Var> push(const std::vector<Var>& args);
typedef decltype(&pop) Special;
}

enum class TreeType{
EXP,
IF,
WHILE,
PRINT,
RETURN,
DEF,
};

class StatementTree;

class ASGrove{
  public:
    class Func{
      private:
      std::shared_ptr<ASGrove> body;
      std::vector<std::string> names;
      public:
      Func() = default;
      Func(const std::vector<Token>& tokens, ASGrove* owner);
      std::optional<Var> operator()(const std::vector<Var>& args) const;
    };
  public:
    static std::vector<std::vector<Var>> array_holder;
    static const std::map<std::string, Specials::Special> specials;
    friend class Func;
  private:
    std::vector<ASTree*> statements;
    std::vector<TreeType> types; //A reference Vector to tell what type the trees are
    std::map<std::string, Var> vars;
    std::map<std::string, Func> funcs;
    
    unsigned place; //how many trees have been executed
    const ASGrove* parent;
    bool is_func;
    ASGrove(std::vector<std::vector<Token>> commands, unsigned start, unsigned end, ASGrove* owner = nullptr, bool func = false);
    std::optional<Var> search_var(const std::string& query) const;
    std::optional<Var> calcHelp(const ASTree::ASNode&);
    void printHelp(const ASTree::ASNode&) const;
    std::optional<Var> find_func(const std::string& name, const std::vector<Var>& args) const;
    void update_existing(const std::map<std::string, Var>&); //If a Variable exists both in the lower grove and upper grove, takes the value from the lower grove and assigns it to the upper grove. (Makes i++) 
    const std::map<std::string, Var>& show_vars() const;
    void add_var(const std::string& name, Var val);
    
public:
  ASGrove();
  ASGrove(const ASGrove&);
  ASGrove(std::vector<std::vector<Token>> commands, ASGrove* owner = nullptr, bool func = false);
  ~ASGrove();
  ASGrove(const ASGrove&, bool temp);
  void clear();
  void reset();
  std::optional<Var> eval(); // Evaluates all trees sequentially until the end
  std::pair<std::optional<Var>, bool> calc(bool print = true); //Stepper evaluates one tree one step at a time
  void add_tree(ASTree* tree, TreeType type = TreeType::EXP);
  void printAll(unsigned indent = 0) const;
  void print(unsigned i, unsigned indent) const;
  void print_curr() const;
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
  void push_back(StatementTree*);
  void clear() {this->body->clear();}
};

#endif
