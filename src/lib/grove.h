#include "parser.h"
#include <vector>
#include <map>
#include <optional>
#include "error.h"

class ASGrove{
  std::vector<ASTree> statements;
  std::map<std::string, double> vars;
  void step();
  public:
    void step(); 
    double eval();
    std::map<std::string, double> getVariables(){
        return vars;
    }
    void addVar(const std::string& name, double val){ //should be called anytime an equal sign operator is reached in the eval phase, no need for passing around chunky nodes, we'll extract the data inside eval
                                                              //plus this would require passing in two tokens, a var and a const but a var can be assigned from another var so better to make it more general i think
      vars.emplace(name, val); //emplace is easier and better, makes pair automatically inside dict with no copying
    }
    std::optional<double> searchVar(const std::string& query){ //definitely shouldn't be in tree, trees are statements so they only evaluate, theres only continuity between trees in the same grove(scope)
                                                               //made optional so that it may or may not have a value, can use std::optional<T>::has_value() method to check
      auto value{vars.find(query)};//map iterator type
      return (value == vars.end()) ? {} : std::optional<double>{value->second}; //returns empty option if not found, option with value otherwise
    }

    double calc(){
      double ret;
      for(size_t i = 0; i < statements.size();i++){
          ret = calcHelp(statements.at(i).getProot());
      }
      return ret;
    }


    double calcHelp(ASTree::ASNode root){
      

    double ret = 0.0; // will be returned
	  switch(curr->pdata.get_type()){

        case TokenType::EQUAL:
            
                double lastVal = pchildren.at(pchildren.size()-1).calcHelp();

                for(size_t j =0; j < this->pchildren.size();j++){
                    
                    //set these children nodes to be lastVal... TODO...
                    //add to variables list

                    
                }
                return lastVal;
                break;
            
		
		case TokenType::EXP:
			
            
            double val;
			for(size_t i =0; i < this->pchildren.size();i++){

                    bool varFlag = 0;
                    
                    if( this->pchildren.at(i).get_pdata().get_type() == TokenType::VAR){
                        varFlag = 1;
                        //  ret = ..... search through variable list to find the right one
                        
                    }else{
				        val = this->pchildren.at(i).calcHelp(); // recursively obtains the value of a child, the children could be an expression or a constant
                    }
                                      
                
				if(i ==0 and !varFlag){ // if the child is the first of its siblings, set the return value to that childs value
					ret = val;
				}else if(!varFlag){
                    
					switch(this->pdata.get_text()[0]){ //math operations

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
            
		    return std::stod(this->pdata.get_text()); // if the token is a constant, just return it casted as a double
            break;
        
        default:
            throw ParserError(this->pdata);
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
