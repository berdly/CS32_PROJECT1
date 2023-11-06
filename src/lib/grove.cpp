#include "grove.h"
#include "error.h"
#include <cmath>
#include "lexer.h"

ASGrove::ASGrove() : statements{}, types{}, vars{}, place{}, parent{nullptr} {}
//ASGrove::ASGrove(const std::vector<ASTree>& tree) : statements{tree}, vars{}, place{} {}
//ASGrove::ASGrove(const ASTree& tree) : statements(std::vector<ASTree>{tree}), vars{}, types{}, place{} {}
ASGrove::ASGrove(std::vector<std::vector<Token>> commands, unsigned start, unsigned end, ASGrove* owner): statements{}, types{}, vars{}, place{}, parent{owner} {
	//needs to be changed to normal number for loop using start and end
	for(unsigned i{start}; i <= end; i++){
		int condition_end{};
		ASTree tree{};
		StatementTree* state{};
		switch(commands.at(i).front().get_type()){
			case TokenType::WHILE:
				for(unsigned j{1}; j < commands.at(i).size() - 2; j++){
					if(commands.at(i).at(j).get_type() == TokenType::LBRACE){
						condition_end = j;
						break;
					}
				}
				if(condition_end == 0){
						throw ParserError(commands.at(i).back());
				}
				if(commands.at(i).back().get_type() != TokenType::RBRACE){
					throw ParserError{commands.at(i).back(), PErrType::END};
				}
				state = new StatementTree{ASTree{commands.at(i), 1, static_cast<unsigned>(condition_end - 1)}, new ASGrove{split_infix(commands.at(i), condition_end + 1, commands.at(i).size() - 2), this}};
				statements.push_back(static_cast<ASTree*>(state));
				types.push_back(TreeType::WHILE);
				break;
			case TokenType::IF:
				for(unsigned j{1}; j < commands.at(i).size() - 2; j++){
					if(commands.at(i).at(j).get_type() == TokenType::LBRACE){
						condition_end = j;
						break;
					}
				}
				if(condition_end == 0){
						throw ParserError(commands.at(i).back());
				}
				if(commands.at(i).back().get_type() != TokenType::RBRACE){
					throw ParserError{commands.at(i).back(), PErrType::END};
				}
				state = new StatementTree{ASTree{commands.at(i), 1, static_cast<unsigned>(condition_end - 1)}, new ASGrove{split_infix(commands.at(i), condition_end + 1, commands.at(i).size() - 2), this}};
				statements.push_back(static_cast<ASTree*>(state));
				types.push_back(TreeType::IF);
				break;
			case TokenType::ELSE:
				if(types.back() != TreeType::IF){
					throw ParserError(commands.at(i).front());
				}
				if(commands.at(i).at(1).get_type() == TokenType::IF){
					for(unsigned j{1}; j < commands.at(i).size() - 2; j++){
						if(commands.at(i).at(j).get_type() == TokenType::LBRACE){
							condition_end = j;
							break;
						}
					}
					if(condition_end == 0){
						throw ParserError(commands.at(i).back());
					}
					tree = ASTree{commands.at(i), static_cast<unsigned>(2), static_cast<unsigned>(condition_end - 1)};
				}
				else{
					condition_end = 1;
					tree = ASTree{std::vector<Token>{Token{0,0,"true", TokenType::BOOL}}};
				}
				if(commands.at(i).back().get_type() != TokenType::RBRACE){
					throw ParserError{commands.at(i).back(), PErrType::END};
				}
				state = static_cast<StatementTree*>(statements.back());
				state->push_back(new StatementTree{ASTree{tree}, new ASGrove{split_infix(commands.at(i), condition_end + 1, commands.at(i).size() - 2), this}});
				break;
			case TokenType::PRINT:
				statements.push_back(new ASTree{commands.at(i), 1, static_cast<unsigned>(commands.at(i).size() - 1)});
				types.push_back(TreeType::PRINT);
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
	for(ASTree* tree: statements){
		delete tree;
	}
}
void ASGrove::reset(){
	place = 0;
}
Var ASGrove::eval(){
      Var val{};
      while(place < statements.size()){
      	val = calc(false);
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
		   auto pval{parent->search_var(query)};
		   return (pval.has_value()) ? pval : std::optional<Var>{};
	   }
	   return std::optional<Var>{};
   }
   else{
	   return std::optional<Var>{value->second};
   }
}

Var ASGrove::calc(bool print){
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
		case TreeType::PRINT:
		case TreeType::EXP:
			ret = calcHelp(tree->getProot());
			break;
		case TreeType::IF:
			statement = static_cast<StatementTree*>(tree);
			while(statement){
				ret = calcHelp(statement->getProot());
				if(!std::holds_alternative<bool>(ret)){
					throw ConditionalError{};
				}
				else if(std::get<bool>(ret)){
					statement->get_body()->eval();
					this->update_existing(statement->get_body()->show_vars());
					statement->clear();
					statement->get_body()->reset();
					break;
				}
				statement = statement->get_next();
			}
			break;
		case TreeType::WHILE:
		    statement = static_cast<StatementTree*>(tree);
			while(true){
				ret = calcHelp(statement->getProot());
				if(!std::holds_alternative<bool>(ret)){
					throw ConditionalError{};
				}
				else if(std::get<bool>(ret)){
					statement->get_body()->eval();
					this->update_existing(statement->get_body()->show_vars());
					statement->clear();
					statement->get_body()->reset();
				}
				else{
					break;
				}
			}
			break;
		}
		if(print || types.at(place) == TreeType::PRINT){
			if(std::holds_alternative<double>(ret)){
				std::cout<<std::get<double>(ret)<<std::endl;
			}
			else if(std::holds_alternative<bool>(ret)){
				std::cout<< std::boolalpha << std::get<bool>(ret) << std::endl;
			}
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
	catch(const ConditionalError&){
		++place;
		vars = backup;
		throw ConditionalError{};
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

      case TokenType::ASSIGN:
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
            
			if(!(std::holds_alternative<double>(val) && std::holds_alternative<double>(ret))){
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
	    ret = true;
		for(const auto& child: children){
			val = this->calcHelp(child); // recursively obtains the value of a child, the children could be an expression or a constant
            
			if(!(std::holds_alternative<bool>(val)&&std::holds_alternative<bool>(ret))){
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
			    ret = (std::get<bool>(ret) != std::get<bool>(val));
                            break;

        	default:
                break;
		    }

		}
		++idx;	
	   }
        	return ret;
    case TokenType::EQUAL:
		for(const auto& child: children){
			val = this->calcHelp(child); // recursively obtains the value of a child, the children could be an expression or a constant
            
			if((idx > 0) && !((std::holds_alternative<bool>(val)&&std::holds_alternative<bool>(ret))||(std::holds_alternative<double>(val)&&std::holds_alternative<double>(ret)))){
				throw TypeError(child.get_pdata());
			}
	    	if(idx ==0){ // if the child is the first of its siblings, set the return value to that childs value
		   		ret = val;
	   		}else{
				 
		   	switch(curr.get_text()[0]){ //math operations

			case '=':
				if(std::holds_alternative<bool>(ret)){
			    	ret = std::get<bool>(ret) == std::get<bool>(val);
				}
				else{
					ret = std::get<double>(ret) == std::get<double>(val);
				}
				break;
			case '!':
			    if(std::holds_alternative<bool>(ret)){
			    	ret = std::get<bool>(ret) != std::get<bool>(val);
				}
				else{
					ret = std::get<double>(ret) != std::get<double>(val);
				}
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
void ASGrove::print_curr() const{
	this->print(place, 0);
	std::cout << '\n';
}
void ASGrove::print(unsigned i, unsigned indent, ASTree* start) const{
  ASTree* tree{statements.at(i)};
  StatementTree* statement;
  
  int tr{};
  if(start == nullptr){
  for(unsigned j{}; j < indent * 4; j++){
	
	std::cout << ' ';
  }

  }
  

	switch(types.at(i)){
		case TreeType::EXP:
			printHelp(tree->getProot());
			break;
		case TreeType::IF:
			if(start!= nullptr){
				statement = static_cast<StatementTree*>(start);
  			}else{
				statement = static_cast<StatementTree*>(tree);
			}
			tr = 0;
			while(statement != nullptr){
			if(tr == 0 && start == nullptr){
				std::cout<<"if "; 
				printHelp(statement->getProot());
				std::cout<<" {"<<std::endl;

				statement->get_body()->printAll(indent + 1);
					for(unsigned j{}; j < indent * 4; j++){
						std::cout << ' ';
  					}
			
			
			statement = statement->get_next();
			if(statement == nullptr){
				std::cout<<'}';
			}else{
				std::cout<<'}'<<std::endl;
			}
			tr++;

			}else if(statement->getProot().get_pdata().get_text() != "true"){

				for(unsigned j{}; j < indent * 4; j++){
				std::cout << ' ';
  				}

				
				std::cout<<"else {"<<std::endl;
				
				indent++;
				for(unsigned j{}; j < indent * 4; j++){
				std::cout << ' ';
  				}

				std::cout<<"if ";
				printHelp(statement->getProot());
				std::cout<<" {"<<std::endl;

				statement->get_body()->printAll(indent + 1);


				for(unsigned j{}; j < indent * 4; j++){
					std::cout << ' ';
  				}
				
			
				std::cout<<'}'<<std::endl;
				
				

				statement = statement->get_next();

				print(i,indent,statement);
					
					statement = nullptr;
				
				indent--;
				

				for(unsigned j{}; j <( indent) * 4; j++){
					std::cout << ' ';
  				}

				

				
				
				


				if(statement == nullptr){
					std::cout<<'}';
				}else{
				std::cout<<'}'<<std::endl;
				}
				tr++;
				

			}else{

				for(unsigned j{}; j < indent * 4; j++){
				std::cout << ' ';
  				}
				std::cout<<"else {"<<std::endl; 
				
				

				statement->get_body()->printAll(indent + 1);

				
				for(unsigned j{}; j < indent * 4; j++){
				std::cout << ' ';
  				}

			
			
			statement = statement->get_next();
			if(statement == nullptr){
				std::cout<<'}';
			}else{
				std::cout<<'}'<<std::endl;
			}
			tr++;

			}
			
			
			/* statement->get_body()->printAll(indent + 1);
			for(unsigned j{}; j < indent * 4; j++){
				std::cout << ' ';
  			}
			
			std::cout<<'}'<<std::endl;
			statement = statement->get_next();
			tr++; */
			
			}
			
			break;
		case TreeType::WHILE:
			statement = static_cast<StatementTree*>(tree);
			std::cout<<"while ";
			printHelp(statement->getProot());
			std::cout<<" {"<<std::endl;
			statement->get_body()->printAll(indent+1);
			for(unsigned j{}; j < indent * 4; j++){
				std::cout << ' ';
  			}
			std::cout<<'}';
			
			break;
		case TreeType::PRINT:
			std::cout<<"print ";
			printHelp(tree->getProot());
			break;
		default:
			break;

		    
		}

		if(start != nullptr){
			std::cout<<std::endl;
		}

	}
	
  



  void ASGrove::printAll(unsigned indent)const{
		
	for(unsigned i{}; i < statements.size(); i++){
      	print(i, indent);
		std::cout<<std::endl;
      }
		
  }
		
  

  void ASGrove::printHelp(const ASTree::ASNode& root) const{ 

    switch(root.get_pdata().get_type()){
    
    	case TokenType::ASSIGN:
		case TokenType::EXP:
		case TokenType::EQUAL: //checks the token type - if it is an expression, more recursion needs to be done on the children of the expression
			
			std::cout << '(';
			for(size_t i =0; i < root.get_kids().size();i++){ //loops through all children of the current node being examined
				  printHelp(root.get_kids().at(i)); //recursive call

				if(i == root.get_kids().size() - 1){
					std::cout << ')'; // if it is the last node in a child list, print a ')'
				}
				else{
					switch(root.get_pdata().get_text()[0]){ // if it isnt at the end of the list print the operator
			case '!':
				std::cout << " != ";
				break;
            case '=':
			  if((root.get_pdata().get_text().size() > 1) && (root.get_pdata().get_text()[1] == '=')){
				std::cout << " == ";
			  }
			  else{
              std::cout<< " = ";
			  }
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

						case '<':
							if(root.get_pdata().get_text()== "<="){
								std:: cout<< " <= ";
							}else{
								std::cout<<" < ";
							}
                            break;
						
						case '>':

							if(root.get_pdata().get_text()== ">="){
								std:: cout<< " >= ";
							}else{
								std::cout<< " > ";
							}
                            break;
						
						case '%':
							std::cout << " % ";
							break;
                        default:
                            break;

					}

				}

			}
            break;
		case TokenType::LOG: //checks the token type - if it is an expression, more recursion needs to be done on the children of the expression
			std::cout << '(';
			for(size_t i =0; i < root.get_kids().size();i++){ //loops through all children of the current node being examined
				  printHelp(root.get_kids().at(i)); //recursive call

				if(i == root.get_kids().size() - 1){
					std::cout << ')'; // if it is the last node in a child list, print a ')'
				}
				else{
					switch(root.get_pdata().get_text()[0]){ // if it isnt at the end of the list print the operator
				
            			case '&':
              				std::cout<< " & ";
              				break;
              
						case '|':
						
							std::cout << " | ";
                            break;

						case '^':
							std::cout<< " ^ ";
							break;

						case '=':
							std::cout << " == ";
                            break;

						case '!':
							std::cout << " != ";
                            break;

                        default:
                            break;

					}

				}

			}
			break;
		case TokenType::BOOL:
			std::cout << root.get_pdata().get_text();
			break;
	    case TokenType::CONST: //if the current token is just a constant, there is no more recursion, just print the constant
		    std::cout<<std::stod(root.get_pdata().get_text());
            break;
	  case TokenType::LBRACE:
	  	std::cout<<root.get_pdata().get_text()<<std::endl;
		//indent += "    ";
		break;
	  case TokenType::RBRACE:
	  	std::cout<<root.get_pdata().get_text()<<std::endl;
		//indent  = indent.;
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
		//auto itr{this->vars.find(pair.first)};
		//if(itr != this->vars.end()){
		vars[pair.first] = pair.second;
		//}
	}
}

void ASGrove::clear(){
	vars.clear();
}
void StatementTree::push_back(StatementTree* child){
	if(next){
		next->push_back(child);
	}
	else{
		next = child;
	}
}
StatementTree::~StatementTree(){
	if(next){
		delete next;
	}
	if(body){
		delete body;
	}
}
void StatementTree::update_down(const std::map<std::string, Var>& vars){
	this->get_body()->update_existing(vars);
	if(next){
		next->update_down(vars);
	}
}
