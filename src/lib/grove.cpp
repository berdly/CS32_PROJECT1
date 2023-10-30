#include "grove.h"
#include "error.h"
#include <cmath>
#include "lexer.h"
ASGrove::ASGrove() : statements{}, types{}, vars{}, place{}, parent{nullptr} {}
//ASGrove::ASGrove(const std::vector<ASTree>& tree) : statements{tree}, vars{}, place{} {}
//ASGrove::ASGrove(const ASTree& tree) : statements(std::vector<ASTree>{tree}), vars{}, types{}, place{} {}
ASGrove::ASGrove(std::vector<std::vector<Token>> commands, unsigned start, unsigned end, ASGrove* owner): statements{}, types{}, vars{}, place{}, parent{owner}{
	//needs to be changed to normal number for loop using start and end
	for(unsigned i{start}; i <= end; i++){
		int pdepth{1};
		int condition_end{};
		switch(commands.at(i).front().get_type()){
			case TokenType::KW:
				if(commands.at(i).at(1).get_type() != TokenType::LPAR){
					throw ParserError(commands.at(i).at(1));
				}
				for(unsigned i{2}; i < commands.at(i).size() - 2; i++){
					switch(commands.at(i).at(i).get_type()){
						case TokenType::LPAR:
							pdepth++;
							break;
						case TokenType::RPAR:
							pdepth--;
							if(pdepth < 0){
								throw ParserError(commands.at(i).at(i));
							}
							break;
						default:
							break;
					}
					if(pdepth == 0){
						condition_end = i;
						break;
					}
				}
				if(pdepth > 0){
					throw ParserError{commands.at(i).back(), PErrType::END};
				}
				if(commands.at(i).at(condition_end + 1).get_type() != TokenType::LBRACE){
					throw ParserError{commands.at(i).at(condition_end + 1)};
				}
				else if(commands.at(i).back().get_type() != TokenType::RBRACE){
					throw ParserError{commands.at(i).back(), PErrType::END};
				}
				statements.push_back(new StatementTree{ASTree{commands.at(i), static_cast<unsigned>(2), static_cast<unsigned>(condition_end - 1)}, ASGrove{split_infix(commands.at(i), condition_end + 2, commands.at(i).size() - 1), 0,0,this}});
				if(commands.at(i).front().get_text() == "if"){
					types.push_back(TreeType::IF);
				}
				else if(commands.at(i).front().get_text() == "while"){
					types.push_back(TreeType::WHILE);
				}
				else{
					throw ParserError(commands.at(i).front());
				}
				break;
				default:
				statements.push_back(new ASTree{commands.at(i)});
				types.push_back(TreeType::EXP);
				break;
			}
		}
	}
ASGrove::ASGrove(std::vector<std::vector<Token>> commands, ASGrove* owner): ASGrove{commands, 0, static_cast<unsigned>(commands.size() - 1), owner} {}

ASGrove::~ASGrove(){
	int idx{};
	for(ASTree* tree: statements){
		if(types.at(idx) == TreeType::EXP){
			delete tree;
		}
		else{
			StatementTree* state{static_cast<StatementTree*>(tree)};
			delete state;
		}
		idx++;
	}
}
void ASGrove::reset(){
	place = 0;
}
Var ASGrove::eval(){
      Var val{};
      while(place < statements.size()){
      	val = calc();
      }
      return val;
}

void ASGrove::add_var(const std::string& name, Var val){
  vars[name] = val;
}

std::optional<Var> ASGrove::search_var(const std::string& query) const{
   auto value{vars.find(query)};//map iterator type
   if(value == vars.end()){
	   if(parent){
		   auto pval{parent->show_vars().find(query)};
		   return (pval == parent->show_vars().end()) ? std::optional<Var>{} : std::optional<Var>{pval->second};
	   }
	   return std::optional<Var>{};
   }
   else{
	   return std::optional<Var>{value->second};
   }
}

Var ASGrove::calc(){
  Var ret{};
  auto backup{vars};
  if(place >= statements.size()){
		  throw std::out_of_range("");
  }
  ASTree* tree{statements.at(place)};
  if(!tree){
	  throw std::runtime_error("BAD GROVE");
  }
  StatementTree* statement;
	try{
	switch(types.at(place)){
		case TreeType::EXP:
			ret = calcHelp(tree->getProot());
			break;
		case TreeType::IF:
			ret = calcHelp(tree->getProot());
			if(!std::holds_alternative<bool>(ret)){
				throw TypeError{statements.at(place)->getProot().get_pdata()};
			}
			else if(std::get<bool>(ret)){
				statement = static_cast<StatementTree*>(tree);
				statement->get_body().eval();
				this->update_existing(statement->get_body().show_vars());
			}
			break;
		case TreeType::WHILE:
			while(true){
				ret = calcHelp(tree->getProot());
				if(!std::holds_alternative<bool>(ret)){
					throw TypeError{statements.at(place)->getProot().get_pdata()};
				}
				else if(std::get<bool>(ret)){
					statement = static_cast<StatementTree*>(tree);
					statement->get_body().eval();
					this->update_existing(statement->get_body().show_vars());
					statement->get_body().reset();
				}
				else{
					break;
				}
			}
			break;
		}
	}
	catch(const ZeroDivision&){
		++place;
		vars = backup;
		throw ZeroDivision{};
	}
	catch(const IdentifierError& e){
		++place;
		vars = backup;
		throw e;
	}
	catch(const TypeError& e){
		++place;
		vars = backup;
		throw e;
	}
	if(std::holds_alternative<double>(ret)){
    std::cout<<std::get<double>(ret)<<std::endl;
	}
	else if(std::holds_alternative<bool>(ret)){
    std::cout<<std::get<bool>(ret)<<std::endl;
	}

	++place;
    return ret; //should return final value of tree and update variables but only once
}

Var ASGrove::calcHelp(const ASTree::ASNode& root){
    
    Var ret{}; // will be returned
    const auto& children{root.get_kids()};
    const Token& curr{root.get_pdata()};
    Var val{};
    unsigned idx{};
    Var value{};
	std::optional<Var> possible_val{};
    
    switch(curr.get_type()){

      case TokenType::EQUAL:
            val = this->calcHelp(children.back());
            for(size_t i{}; i < children.size() - 1 ; i++){
                  this->add_var(children.at(i).get_pdata().get_text(), val);
            }

            return val;

            break;
		
     case TokenType::EXP:
	    ret = 0.0;
		for(const auto& child: children){
		    val = this->calcHelp(child); // recursively obtains the value of a child, the children could be an expression or a constant
            
			if(!(std::holds_alternative<double>(val) || std::holds_alternative<double>(ret))){
				throw TypeError(child.get_pdata());
			}
	    	if(idx ==0){ // if the child is the first of its siblings, set the return value to that childs value
		   		ret = val;
	   		}else{
				 
		   switch(curr.get_text()[0]){ //math operations

			case '*':
			    ret = std::get<double>(ret) * std::get<double>(val);
                            break;
			case '+':
			    ret = std::get<double>(ret) + std::get<double>(val);
                            break;
			case '-':
			    ret = std::get<double>(ret) - std::get<double>(val);
                            break;
			case '/':
			    if(std::get<double>(val) == 0){
				throw ZeroDivision{};
			    }
			    ret = std::get<double>(ret) / std::get<double>(val);
                            break;
			case '%':
			    ret = std::fmod(std::get<double>(ret), std::get<double>(val));
			    break;
			case '<':
				if(curr.get_text()== "<="){
					ret = std::get<double>(ret) <= std::get<double>(val);
				}else{
					ret = std::get<double>(ret) < std::get<double>(val);
				}
				break;
			case '>':
				if(curr.get_text()== ">="){
					ret = std::get<double>(ret) >= std::get<double>(val);
				}else{
					ret = std::get<double>(ret) > std::get<double>(val);
				}
				break;
			

        		default:
                            break;
		    }
		}
		++idx;

	}
        	return ret;

    case TokenType::LOG:
		for(const auto& child: children){
			val = this->calcHelp(child); // recursively obtains the value of a child, the children could be an expression or a constant
            
			if(!(std::holds_alternative<bool>(val)||std::holds_alternative<bool>(ret))){
				throw TypeError(child.get_pdata());
			}
	    	if(idx ==0){ // if the child is the first of its siblings, set the return value to that childs value
		   		ret = val;
	   		}else{
				 
		   		switch(curr.get_text()[0]){ //math operations

			case '&':
			    ret = std::get<bool>(ret) && std::get<bool>(val);
                            break;
			case '|':
			    ret = std::get<bool>(ret) || std::get<bool>(val);
                            break;
			case '^':
			    ret = (std::get<bool>(ret) ? std::get<bool>(val) : !(std::get<bool>(val)));
                            break;
			case '=':
			    ret = std::get<bool>(ret) == std::get<bool>(val);
                            break;
			case '!':
			    ret = std::get<bool>(ret) != std::get<bool>(val);
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
    case TokenType::BOOL:
	    return curr.get_text()[0] == 't';
    case TokenType::VAR:
	     possible_val = this->search_var(curr.get_text()); //added this auto - might not work as intended
	     if(possible_val.has_value()){
		 return *possible_val;
	     }
	     else{
		 throw IdentifierError(curr);
	     }
    default:
            //throw ParserError(root.get_pdata());
            break;

		
	}
    return Var{};

  }

  void ASGrove::print() const {
	 
      	  printHelp(statements.at(place)->getProot());
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
		    throw ParserError(root.get_pdata());
            break;

			
	}
  
  }
void ASGrove::add_tree(ASTree* tree, TreeType type){
	this->statements.push_back(tree);
	this->types.push_back(type);
}

const std::map<std::string, Var>& ASGrove::show_vars() const { return vars; }

void ASGrove::update_existing(const std::map<std::string, Var>& v_map) {
	for(const auto& pair: v_map){
		auto itr{this->vars.find(pair.first)};
		if(itr != this->vars.end()){
			itr->second = pair.second;
		}
	}
}
