#include "grove.h"
#include "error.h"
ASGrove::ASGrove() : statements{}, vars{}, place{} {}
ASGrove::ASGrove(const std::vector<ASTree>& tree) : statements{tree}, vars{}, place{} {}
ASGrove::ASGrove(const ASTree& tree) : statements(std::vector<ASTree>{tree}), vars{}, place{} {}

double ASGrove::eval(){
  
      double val = calc();
      return val;
}

void ASGrove::add_var(const std::string& name, double val){
  vars[name] = val;
}

std::optional<double> ASGrove::search_var(const std::string& query){
   auto value{vars.find(query)};//map iterator type
   return (value == vars.end()) ? std::optional<double>{} : std::optional<double>{value->second};
}

double ASGrove::calc(){
  double ret{};
  if(place >= statements.size()){
		  throw std::out_of_range("");
	  }
	try{
	  ret = calcHelp(statements.at(place).getProot());
	}
	catch(const ZeroDivisionError&){
		++place;
		throw ZeroDivisionError{};
	}
	catch(const IdentifierError& e){
		++place;
		throw e;
	}
    std::cout<<ret<<std::endl;
	  ++place;
    return ret; //should return final value of tree and update variables but only once
}

double ASGrove::calcHelp(const ASTree::ASNode& root){
    
    double ret{}; // will be returned
    const auto& children{root.get_kids()};
    const Token& curr{root.get_pdata()};
    double val{};
    unsigned idx{};
    std::optional<double> value{};
    
    switch(curr.get_type()){

      case TokenType::EQUAL:
            val = this->calcHelp(children.back());
	    //std::cout  << "in calcHelp()\n" << children.back().get_pdata().get_text() << '\n';
            for(size_t i{}; i < children.size() - 1 ; i++){
                  this->add_var(children.at(i).get_pdata().get_text(), val);
                    //set these children nodes to be lastVal... TODO...
                    //add to variables list
            }

            return val;

            break;
		
     case TokenType::EXP:
		    for(const auto& child: children){
		    val = this->calcHelp(child); // recursively obtains the value of a child, the children could be an expression or a constant
                                            
	    	if(idx ==0){ // if the child is the first of its siblings, set the return value to that childs value
		   		ret = val;
	   		}else{         
		   		switch(curr.get_text()[0]){ //math operations

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
		++idx;	
	   }
        return ret;
    
           
            
    case TokenType::CONST:
            
	    return std::stod(curr.get_text()); // if the token is a constant, just return it casted as a double
            break;
    case TokenType::VAR:
	     value = this->search_var(curr.get_text());
	     if(value.has_value()){
		 return *value;
	     }
	     else{
		 throw IdentifierError(curr);
	     }
    default:
            //throw ParserError(root.get_pdata());
            break;

		
	}
    return -1.0;

  }

  void ASGrove::print() const {
	 
      		printHelp(statements.at(place).getProot());
          std::cout<<std::endl;
          
  }

  void ASGrove::printHelp(const ASTree::ASNode& root) const{ 

    switch(root.get_pdata().get_type()){
    
    case TokenType::EQUAL:
		case TokenType::EXP: //checks the token type - if it is an expression, more recursion needs to be done on the children of the expression
			std::cout << '(';
			for(size_t i =0; i < root.get_kids().size();i++){ //loops through all children of the current node being examined
				  printHelp(root.get_kids().at(i)); //recursive call

				if(i == root.get_kids().size() - 1){
					std::cout << ')'; // if it is the last node in a child list, print a ')'
				}
				else{
					switch(root.get_pdata().get_text()[0]){ // if it isnt at the end of the list print the operator
				
            case '=':
              std::cout<< " = ";
              break;
              
						case '*':
						
							std::cout << " * ";
                            break;

						case '+':
						
							std::cout << " + ";
                            break;

						case '-':

							std::cout << " - ";
                            break;

						case '/':
							std::cout << " / ";
                            break;

                        default:
                            break;

					}

				}

			}
            break;

		
	    case TokenType::CONST: //if the current token is just a constant, there is no more recursion, just print the constant
		    std::cout<<std::stod(root.get_pdata().get_text());
            break;
      case TokenType::VAR:
        std::cout<<root.get_pdata().get_text();
            break;

      default:
        //std::cout<<"THROW13"<<std::endl;
		    throw ParserError(root.get_pdata());
            break;

			
	}
  
  }
void ASGrove::add_tree(const ASTree& tree){
	this->statements.push_back(tree);
}
