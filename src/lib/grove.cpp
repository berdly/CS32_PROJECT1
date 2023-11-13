#include "grove.h"
#include "error.h"
#include <cmath>
#include "lexer.h"
std::vector<std::vector<Var>> ASGrove::array_holder{};
Var Specials::pop(const std::vector<Var>& args){
  if(args.size() != 1){
    throw std::runtime_error("Runtime error: incorrect argument count.");
  }
  if(!(args[0].holds_Arr())){
    throw std::runtime_error("Runtime error: not an array.");
  }
  if(args[0].get_Arr().size() == 0){
	throw std::runtime_error("Runtime error: underflow.");
  }
  std::vector<Var>& arr{args[0].get_Arr().get()};
  Var last = arr.back();
  arr.pop_back();
  return last;
}
Var Specials::len(const std::vector<Var>& args){
  if(args.size() != 1){
    throw std::runtime_error("Runtime error: incorrect argument count.");
  }
  if(!(args[0].holds_Arr())){
    throw std::runtime_error("Runtime error: not an array.");;
  }
  return Var(static_cast<double>(args[0].get_Arr().size()));
}
Var Specials::push(const std::vector<Var>& args){
   if(args.size() != 2){
    throw std::runtime_error("Runtime error: incorrect argument count.");
  }
  if(!(args[0].holds_Arr())){
    throw std::runtime_error("Runtime error: not an array.");
  }
  args[0].get_Arr().get().push_back(args[1]);
  return Var{};
}

const std::map<std::string, Specials::Special> ASGrove::specials{{{"pop", Specials::pop}, {"len", Specials::len}, {"push", Specials::push}}};
ASGrove::ASGrove(const ASGrove& grove): statements{grove.statements}, types{grove.types}, vars{grove.vars}, funcs{grove.funcs}, place{grove.place}, parent{grove.parent}, is_func{grove.is_func} {}

ASGrove::ASGrove() : statements{}, types{}, vars{}, funcs{}, place{}, parent{nullptr}, is_func{false} {}
ASGrove::ASGrove(const ASGrove& grove, bool temp) : ASGrove{grove} {
	this->is_func = temp;
	auto p = this->parent;
	this->new_owner(this);
	this->parent = p;
}

void ASGrove::new_owner(ASGrove* owner){
	is_func = true;
	parent = owner;
	for(unsigned i{}; i < statements.size(); i++){
		switch(types.at(i)){
			case TreeType::IF:
			case TreeType::WHILE:
				statements.at(i) = static_cast<ASTree*>(new StatementTree{*(static_cast<StatementTree*>(statements.at(i))), this});
				break;
			default:
				break;
		}
	}
}
//ASGrove::ASGrove(const std::vector<ASTree>& tree) : statements{tree}, vars{}, place{} {}
//ASGrove::ASGrove(const ASTree& tree) : statements(std::vector<ASTree>{tree}), vars{}, types{}, place{} {}
ASGrove::ASGrove(std::vector<std::vector<Token>> commands, unsigned start, unsigned end, ASGrove* owner, bool func): statements{}, types{}, vars{}, funcs{}, place{}, parent{owner}, is_func{func} {
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
			case TokenType::RETURN:
				if(commands.at(i).size() == 1){
					if(commands.at(i).front().get_text() == ""){
						statements.push_back(new ASTree{Token{0,0," ", TokenType::VOID}});
					}
					else{
						statements.push_back(new ASTree{Token{0,0,"", TokenType::VOID}});
					}
				}
				else{
					statements.push_back(new ASTree{commands.at(i), 1, static_cast<unsigned>(commands.at(i).size() - 1)});
				}
				types.push_back(TreeType::RETURN);
				break;
			case TokenType::DEF:
				
				if((commands.at(i).at(1).get_type() != TokenType::VAR) && (commands.at(i).at(1).get_type() != TokenType::CONST) && (commands.at(i).at(1).get_type() != TokenType::BOOL)){
					throw ParserError{commands.at(i).at(1)};
				}
				statements.push_back(new ASTree{commands.at(i).at(1)});
				types.push_back(TreeType::DEF);
				funcs.emplace(std::make_pair(commands.at(i).at(1).get_text(), Func{commands.at(i), this}));
				add_var(commands.at(i).at(1).get_text(), Var{static_cast<Func*>(&funcs[commands.at(i).at(1).get_text()])});
				break;
			default:
				statements.push_back(new ASTree{commands.at(i)});
				types.push_back(TreeType::EXP);
				break;
			}
		}
	}
ASGrove::ASGrove(std::vector<std::vector<Token>> commands, ASGrove* owner, bool func): ASGrove{commands, 0, static_cast<unsigned>(commands.size() - 1), owner, func} {}

ASGrove::~ASGrove(){
	if(!is_func){
		for(ASTree* tree: statements){
			delete tree;
		}
	}
	else{
		for(unsigned i{}; i < statements.size(); i++){
			if((types.at(i) == TreeType::IF) || (types.at(i) == TreeType::WHILE)){
				delete statements.at(i);
			}
		}
	}
}
void ASGrove::reset(){
	place = 0;
}
Var ASGrove::eval(){
      while(place < statements.size()){
      	auto val = calc(false);
		if(val.second){
			return val.first;
		}
      }
      return Var{};
}

void ASGrove::add_var(const std::string& name, Var val){
  vars[name] = val;
}

std::pair<Var, bool> ASGrove::search_var(const std::string& query) const{
   auto value{vars.find(query)};//map iterator type
   if(value == vars.end()){
	   if(parent){
		   auto pval{parent->search_var(query)};
		   return pval.second ? std::make_pair(pval.first, true) : std::make_pair(Var{}, false);
	   }
   }
   else{
	   return std::make_pair(Var{value->second}, true);
   }
   auto special{specials.find(query)};
   if(special != specials.end()){
	   return std::make_pair(special->second, true);
   }
   return std::make_pair(Var{}, false);
}

std::pair<Var, bool> ASGrove::calc(bool print){
  Var possible_val{};
  auto backup{vars};
  if(place >= statements.size()){
		  throw std::out_of_range("");
  }
  ASTree* tree{statements.at(place)};
  if(!tree){
	  throw std::runtime_error("BAD GROVE");
  }
  TreeType type{types.at(place)};
  StatementTree* statement;
	try{
	switch(type){
		case TreeType::PRINT:
		case TreeType::EXP:
		case TreeType::RETURN:
			possible_val = calcHelp(tree->getProot());
			break;
		case TreeType::IF:
			statement = static_cast<StatementTree*>(tree);
			while(statement){
				possible_val = calcHelp(statement->getProot());
				if(!possible_val.has_value()){
					throw ConditionalError{};
				}
				if(!possible_val.holds_bool()){
					throw ConditionalError{};
				}
				else if(possible_val.get_bool()){
					possible_val = statement->get_body()->eval();
					
					this->update_existing(statement->get_body()->show_vars());
					statement->clear();
					statement->get_body()->reset();
					if(possible_val.has_value()){
						type = TreeType::RETURN;
					}
					break;
				}
				statement = statement->get_next();
			}
			break;
		case TreeType::WHILE:
		    statement = static_cast<StatementTree*>(tree);
			while(true){
				possible_val = calcHelp(statement->getProot());
				if(!possible_val.has_value()){
					throw ConditionalError{};
				}
				if(!possible_val.holds_bool()){
					throw ConditionalError{};
				}
				else if(possible_val.get_bool()){
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
		case TreeType::DEF:
			funcs[tree->getProot().get_pdata().get_text()].enclose(vars);
			break;
		}
		if(print || (types.at(place) == TreeType::PRINT)){
			std::cout << possible_val << '\n';
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
	if(type == TreeType::RETURN){
		if(!is_func){
			throw std::runtime_error("Runtime error: unexpected return.");
		}
		return std::make_pair(possible_val, true);
	}
	return std::make_pair(possible_val, false);
}

Var ASGrove::calcHelp(const ASTree::ASNode& root){
    //TODO make optional due to new void return types
    Var ret{}; // will be returned
    const auto& children{root.get_kids()};
    const Token& curr{root.get_pdata()};
    unsigned idx{};
	std::vector<Var> args;
	Var possible_val{};
    double dummy;
	std::pair<Var, bool> result;

    switch(curr.get_type()){
      case TokenType::ASSIGN:
            possible_val = this->calcHelp(children.back());
            for(size_t i{}; i < children.size() - 1 ; i++){
				if(children.at(i).get_pdata().get_type() == TokenType::RBRACK){
					ret = calcHelp(children.at(i).get_kids().at(1));
					if(!ret.holds_double()){
						throw std::runtime_error("Runtime error: index is not a number.");
					}
					if(std::modf(ret.get_double(), &dummy) != 0){
						throw std::runtime_error("Runtime error: index is not an integer.");
					}
					dummy = ret.get_double();
					ret = calcHelp(children.at(i).get_kids().at(0));
					if(!ret.holds_Arr()){
							throw std::runtime_error("Runtime error: not an array.");
					}
					if((static_cast<size_t>(dummy) > (ret.get_Arr().size() - 1)) || (dummy < 0)){
						throw std::runtime_error("Runtime error: index out of bounds.");
					}
					ret.get_Arr().at(static_cast<unsigned>(dummy)) = possible_val; 
				}
				else if (children.at(i).get_pdata().get_type() == TokenType::VAR){
                	this->add_var(children.at(i).get_pdata().get_text(), possible_val);
				}
				else{
					throw std::runtime_error("Runtime error: invalid assignee.");
				}
            }
            return possible_val;
     case TokenType::EXP:
	    ret = 0.0;
		for(const auto& child: children){
		    possible_val = this->calcHelp(child); // recursively obtains the value of a child, the children could be an expression or a constant

			if(!(possible_val.holds_double() && ret.holds_double())){
				throw TypeError(child.get_pdata());
			}
	    	if(idx ==0){ // if the child is the first of its siblings, set the return value to that childs value
		   		ret = possible_val;
	   		}else{
				 
		   switch(curr.get_text()[0]){ //math operations

			case '*':
			    ret = ret.get_double() * possible_val.get_double();
                            break;
			case '+':
			    ret = ret.get_double() + possible_val.get_double();
                            break;
			case '-':
			    ret = ret.get_double() - possible_val.get_double();
                            break;
			case '/':
			    if(possible_val.get_double() == 0){
				throw ZeroDivision{};
			    }
			    ret = ret.get_double() / possible_val.get_double();
                            break;
			case '%':
			    ret = std::fmod(ret.get_double(), possible_val.get_double());
			    break;
			case '<':
				if(curr.get_text()== "<="){
					ret = ret.get_double() <= possible_val.get_double();
				}else{
					ret = ret.get_double() < possible_val.get_double();
				}
				break;
			case '>':
				if(curr.get_text()== ">="){
					ret = ret.get_double() >= possible_val.get_double();
				}else{
					ret = ret.get_double() > possible_val.get_double();
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
			possible_val = this->calcHelp(child); // recursively obtains the value of a child, the children could be an expression or a constant
			if(!(possible_val.holds_bool() && ret.holds_bool())){
				throw TypeError(child.get_pdata());
			}
	    	if(idx ==0){ // if the child is the first of its siblings, set the return value to that childs value
		   		ret = possible_val;
	   		}else{
				 
		   		switch(curr.get_text()[0]){ //math operations

			case '&':
			    ret = ret.get_bool() && possible_val.get_bool();
                            break;
			case '|':
			    ret = ret.get_bool() || possible_val.get_bool();
                            break;
			case '^':
			    ret = (ret.get_bool() != possible_val.get_bool());
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
			possible_val = this->calcHelp(child); // recursively obtains the value of a child, the children could be an expression or a constant
	    	if(idx ==0){ // if the child is the first of its siblings, set the return value to that childs value
		   		ret = possible_val;
	   		}else{
				 
		   	switch(curr.get_text()[0]){ //math operations

			case '=':
			    ret = (ret == possible_val);
				break;
			case '!':
			    ret = (ret != possible_val);
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
	     result = this->search_var(curr.get_text()); //added this auto - might not work as intended
	     if(result.second){
		  	return result.first;
	     }
	     else{
		 	throw IdentifierError(curr);
	     }
	case TokenType::VOID:
		return Var{};
	case TokenType::LBRACK:
		array_holder.emplace_back(children.size());
		idx = static_cast<unsigned>(array_holder.size() - 1);
		for(unsigned i{}; i < children.size(); i++){
			array_holder.at(idx).at(i) = this->calcHelp(children.at(i));;
		}
		return Arr{idx};
	case TokenType::RBRACK:
		possible_val = calcHelp(children.at(0)); //added this auto - might not work as intended
		if(!possible_val.holds_Arr()){
			throw std::runtime_error("Runtime error: not an array.");
		}
		ret = calcHelp(children.at(1));
		
		if(!ret.holds_double()){
			throw std::runtime_error("Runtime error: index is not a number.");
		}
		if(std::modf(ret.get_double(), &dummy) != 0){
			throw std::runtime_error("Runtime error: index is not an integer.");
		}
		if((static_cast<size_t>(ret.get_double()) > (possible_val.get_Arr().size() - 1)) || (ret.get_double() < 0)){
			throw std::runtime_error("Runtime error: index out of bounds.");
		}
		return possible_val.get_Arr().at(static_cast<unsigned>(ret.get_double()));
	case TokenType::LPAR:
		for(unsigned i{1}; i < children.size(); i++){
			args.push_back(this->calcHelp(children.at(i)));
		}
		possible_val = this->calcHelp(children.at(0));
		if(possible_val.holds_Func()){
			return possible_val.get_Func()->operator()(args);
		}
		else if(possible_val.holds_Special()){
			return possible_val.get_Special()(args);
		}
		else{
			throw std::runtime_error("Runtime error: not a function.");
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
void ASGrove::print_line() const{
	ASTree * tree{statements.at(place)};
	printHelp(tree->getProot());
	std::cout << '\n';
}

void ASGrove::print(unsigned i, unsigned indent, ASTree* start) const{
  ASTree* tree{statements.at(i)};
  StatementTree* statement;
  int tr{};
  std::pair<Var, bool> funcSearch;
  Func* fun;
  
  
  
  if(start == nullptr){
  for(unsigned j{}; j < indent * 4; j++){
	
	std::cout << ' ';
  }

  }
	switch(types.at(i)){
		case TreeType::EXP:
			printHelp(tree->getProot());
			std::cout<<";";
			break;
		case TreeType::DEF:
			std::cout<<"def ";
			printHelp(tree->getProot());
			std::cout<<"(";

			funcSearch = search_var(tree->getProot().get_pdata().get_text());
			
			if((funcSearch.first).holds_Func()){
				fun = funcSearch.first.get_Func();
			}
			


			for(size_t i = 0; i < fun->get_names().size(); i++){
				
				if(i != fun->get_names().size()-1){
					std::cout<< fun->get_names().at(i)<<", ";
				}else{
					std::cout<<fun->get_names().at(i);
				}
			}
			std::cout<<") {\n";
 
			fun->get_body()->printAll(indent + 1);

			for(unsigned j{}; j < indent * 4; j++){
				std::cout << ' ';
  				}
			std::cout<<"}";
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
				if(statement != nullptr){
					print(i,indent,statement);	
				}

				
				
				indent--;
				

				for(unsigned j{}; j <( indent) * 4; j++){
					std::cout << ' ';
  				}

				

			
				
				std::cout<<'}';
				
				tr++;
				break;
				

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
			std::cout<<";";
			break;
		case TreeType::RETURN:
			std::cout<<"return ";
			printHelp(tree->getProot());
			std::cout<<";";
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
	  case TokenType::LPAR:

		std::cout<<root.get_kids().at(0).get_pdata().get_text()<<"(";
		for(size_t i = 1; i < root.get_kids().size();i++){
			printHelp(root.get_kids().at(i));

			if(i != root.get_kids().size()-1){
				std::cout<< ", ";
			}
		}
		std::cout<<")";

	  	break;
	  case TokenType::LBRACK:
	  	std::cout<<"[";

		for(size_t i = 0; i < root.get_kids().size();i++){
			printHelp(root.get_kids().at(i));

			if(i != root.get_kids().size()-1){
				std::cout<< ", ";
			}
		}
		std::cout<<"]";

	  	break;
	
	 case TokenType::RBRACK:

		std::cout<< root.get_kids().at(0).get_pdata().get_text()<<"[";
		printHelp(root.get_kids().at(1));
		std::cout<<"]";

		break;

		
      case TokenType::VAR:
        std::cout<<root.get_pdata().get_text();
            break;
	
	  case TokenType::VOID:
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

Var ASGrove::find_func(const std::string& name, const std::vector<Var>& args) const {
	auto result{search_var(name)};
	if(result.second){
		if(!result.first.holds_Func()){
			throw std::runtime_error("Runtime Error: not a function");
		}
		return result.first.get_Func()->operator()(args);
	}
	if(parent){
		return parent->find_func(name, args);
	}
	auto special{specials.find(name)};
	if(special != specials.end()){
		return special->second(args);
	}
	throw std::runtime_error(std::string("Runtime Error: unknown identifier ") + name);
}

Func::Func(const std::vector<Token>& tokens, ASGrove* owner): body{}, names{} {
    unsigned var_end{};
    if(tokens.at(2).get_type() != TokenType::LPAR){
      throw ParserError{tokens.at(2)};
    }
    TokenType last{TokenType::COMMA};
    for(unsigned i{3}; i < tokens.size(); i++){
      switch(tokens.at(i).get_type()){
        case TokenType::RPAR:
           var_end = i;
           break;
        case TokenType::VAR:
          if(last == TokenType::VAR){
            throw ParserError(tokens.at(i));
          }
          names.push_back(tokens.at(i).get_text());
          last = TokenType::VAR;
          break;
        case TokenType::COMMA:
          last = TokenType::COMMA;
          break;
        default:
          throw ParserError(tokens.at(i));
          break;
      }
      if(var_end > 0){
        break;
      }
    }
	//test
    if(var_end == 0){
      throw ParserError(tokens.back());
    }
    if(tokens.at(var_end + 1).get_type() != TokenType::LBRACE){
      throw ParserError(tokens.at(var_end + 1));
    }
    if(tokens.back().get_type() != TokenType::RBRACE){
      throw ParserError(tokens.back());
    }
	if(var_end + 2 == tokens.size() - 1){
		body.reset(new ASGrove{std::vector<std::vector<Token>>{std::vector{Token{0,0,"", TokenType::RETURN}}}});
	}
    else{
		body.reset(new ASGrove{split_infix(tokens, var_end + 2, static_cast<unsigned>(tokens.size() - 2)), owner, false});
	}
  }
Var Func::operator()(const std::vector<Var>& args) const{
    if(args.size() != names.size()){
      throw std::runtime_error("Runtime error: incorrect argument count.");
    }
    ASGrove new_scope{*(this->body), true}; //copies body by value to create new scope
    for(unsigned i{}; i < names.size(); i++){
      new_scope.add_var(names.at(i), args.at(i));
    }
    return new_scope.eval();
  }
void Func::enclose(const std::map<std::string, Var>& new_vars){
	for(const auto& var: new_vars){
		this->body->vars.insert_or_assign(var.first, var.second);
	}
}
StatementTree::StatementTree(const StatementTree& tree, ASGrove* owner): ASTree{tree.proot}, body{tree.body}, next{tree.next} {
	body = new ASGrove{*body};
	body->new_owner(owner);
	if(next){
		next = new StatementTree(*next, owner);
	}
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
Arr::Arr(unsigned num) : idx{num} {}
 bool Arr::operator==(const Arr& other){
	return idx == other.idx;
 }
  //std::vector<Var>& get();
  Var& Arr::at(unsigned num){
	return ASGrove::array_holder.at(idx).at(num);
  }
  unsigned Arr::size(){
	return ASGrove::array_holder.at(idx).size();
  }
  Var& Arr::back(){
	return ASGrove::array_holder.at(idx).back();
  }
  Var& Arr::front(){
	return ASGrove::array_holder.at(idx).front();
  }
  std::vector<Var>& Arr::get(){
	return ASGrove::array_holder.at(idx);
  }
