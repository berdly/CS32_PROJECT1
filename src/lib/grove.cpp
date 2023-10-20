#include "grove.h"

ASGrove::ASGrove(const std::vector<ASTree>& tree) : statements{tree}, vars{}, place{} {}
ASGrove::ASGroce(const ASTree& tree) : statements(std::vector<ASTree>{tree}, vars{}, place{} {}

double ASGrove::eval(){
  double val{};
  for(size_t i{}; i < statements.size(); i++){
      val = calc();
  }
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
  if(place >= statements.size()){
		  throw std::out_of_range("");
	  }
	  double ret{calcHelp(statements.at(place).getProot())};
	  ++place;
    return ret; //should return final value of tree and update variables but only once
}

double ASGrove::calcHelp(const ASTree::ASNode& root){
      
    double ret{}; // will be returned
    const auto& children{root.get_kids()};
    const Token& curr{root.get_pdata()};
    double val{};
    unsigned idx{};
  
    switch(curr.get_type()){

      case TokenType::EQUAL:
            val = this->calcHelp(children.back());
            for(size_t i{}; i < children.size() - 1 ; i++){
                  this->add_var(children.at(i).get_pdata().get_text(), val);
                    //set these children nodes to be lastVal... TODO...
                    //add to variables list
                }

            return val;

            break;
		
     case TokenType::EXP:
		for(const auto& child: children){
                    
            if(child.get_pdata().get_type() == TokenType::VAR){
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
		++idx;	
	   }
        return ret;
    
           
            
    case TokenType::CONST:
            
	    return std::stod(root.get_pdata().get_text()); // if the token is a constant, just return it casted as a double
            break;
        
    default:
            throw ParserError(root.get_pdata());
            break;

		
	}
    return -1.0;

  }

  void ASGrove::print() const {
	  for(size_t i = 0; i < statements.size();i++){
      		printHelp(statements.at(i).getProot());
          }
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
