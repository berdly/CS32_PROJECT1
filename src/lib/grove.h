#include "parser.h"
#include <vector>
#include <map>
#include <optional>
#include "error.h"

class ASGrove{
  std::vector<ASTree> statements;
  std::map<std::string, double> vars;
  unsigned place; //how many trees have been executed
  void step();
  public:
    void step(); 
    double eval();
    std::map<std::string, double> getVariables(){
        return vars;
    }
    void add_var(const std::string& name, double val){ //should be called anytime an equal sign operator is reached in the eval phase, no need for passing around chunky nodes, we'll extract the data inside eval
                                                              //plus this would require passing in two tokens, a var and a const but a var can be assigned from another var so better to make it more general i think
      vars[name] = val; //this syntax allows for overwriting
    }
    std::optional<double> search_Var(const std::string& query){ //definitely shouldn't be in tree, trees are statements so they only evaluate, theres only continuity between trees in the same grove(scope)
                                                               //made optional so that it may or may not have a value, can use std::optional<T>::has_value() method to check
      auto value{vars.find(query)};//map iterator type
      return (value == vars.end()) ? {} : std::optional<double>{value->second}; //returns empty option if not found, option with value otherwise
    }

    double calc(){
	if(place >= statements.size()){
		throw std::out_of_range{};
	}
	double ret{calcHelp(statements.at(place).getProot());
	++place;
      return ret; //should return final value of tree and update variables but only once
    }


    double calcHelp(ASTree::ASNode root){
      

    double ret = 0.0; // will be returned
	    const Token& curr{root.get_pdata()};
	  switch(curr.get_type()){
	
        case TokenType::EQUAL:
		auto children{curr.get_kids()};
		/*
		for(unsigned i{}; i < (children.size() - 1); i++){
			if(children[0].get_type() != TokenType::VAR){
			throw ParserError{children[0]};
		}
  */ //pro
                double lastVal{this->calcHelp(children.back())};

                for(size_t i{}; i < children.size() - 1 ;j++){
                    this->add_var(children.at(i).get_pdata(), lastVal);
                    //set these children nodes to be lastVal... TODO...
                    //add to variables list
                }
                return lastVal;
                break;
		
		case TokenType::EXP:
			auto children{curr.get_kids()};
			
            
            double val{};
	    unsigned idx{};
	    for(const auto& child: children){

                    bool varFlag{false};
                    
                    if( child.get_pdata().get_type() == TokenType::VAR){
                        varFlag = 1;
			std::optional<double> value{this->search_var(child.get_pdata().get_text())};
			if(value.has_value()){
				val = *value;
			}
			else{
				throw ParserError(child.get_pdata());
			}
                        
                    }else{
				        val = this->calcHelp(child); // recursively obtains the value of a child, the children could be an expression or a constant
                    }
                                      
                
				if(idx ==0){ // if the child is the first of its siblings, set the return value to that childs value
					ret = val;
				}else{
                    
					switch(root.get_pdata().get_text()[0]){ //math operations

						case '*':
							ret *=val;
                            break;
						case '+':
							ret += val;
                            break;
						case '-':
							ret -= val;
                            break;
						case '/':
							if(val == 0){
								throw ZeroDivision{};
							}
							ret /= val;
                            break;
                        default:
                            break;
					}
				}
				
			}
            return ret;
            break;
	    case TokenType::CONST:
            
		    return std::stod(root.get_pdata().get_text()); // if the token is a constant, just return it casted as a double
            break;
        
        default:
            throw ParserError(root.get_pdata());
            break;

		
	}
    return -1;

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
