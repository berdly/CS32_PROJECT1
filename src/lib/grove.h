#include "parser.h"
#include <vector>
#include <map>

class ASGrove<template class T>{
  std::vector<ASTree> statements;
  std::map<std::string, double> variables;
  void step();
  public:
  T eval();
}
