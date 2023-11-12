#include "parser.h"
#include "error.h"
#include <stack>
#include <iostream>
#include <stdexcept>
ASTree::ASTree() : proot{} {};
ASTree::ASTree(const Token& token) : proot{ASTree::ASNode{token}} {}
ASTree::ASTree(const ASTree::ASNode& node): proot{node} {}
bool wrapped(const std::vector<Token>& tokens, unsigned start, unsigned end){
	int pdepth{};
	if(tokens.empty()){
		return false;
	}
	if(tokens.at(start).get_type() == TokenType::LPAR){
		for(unsigned i{start+ 1}; i <= end; i++){
			if(tokens.at(i).get_type() == TokenType::LPAR){
				pdepth++;
			}
			else if(tokens.at(i).get_type() == TokenType::RPAR){
				if(pdepth == 0){
					return (i == end) && (end > 1);
				}
				
				else{
					pdepth--;
				}
		}
	}
	}
	else{
		return false;
	}
	return false;
}
bool wrapped_bracks(const std::vector<Token>& tokens, unsigned start, unsigned end){
	int pdepth{};
	if(tokens.empty()){
		return false;
	}
	if(tokens.at(start).get_type() == TokenType::LBRACK){
		for(unsigned i{start+ 1}; i <= end; i++){
			if(tokens.at(i).get_type() == TokenType::LBRACK){
				pdepth++;
			}
			else if(tokens.at(i).get_type() == TokenType::RBRACK){
				if(pdepth == 0){
					return (i == end) && (end > 1);
				}
				
				else{
					pdepth--;
				}
		}
	}
	}
	else{
		return false;
	}
	return false;
}
ASTree::ASNode ASTree::build_array(const std::vector<Token>& tokens, unsigned start, unsigned end){
	ASNode root{tokens.at(start)};
	int curr_start{static_cast<int>(start + 1)};
	if(start == end){
		return root;
	}
	int bdepth{};
	int pdepth{};
	for(unsigned i{start + 1}; i <= end; i++){
		switch(tokens.at(i).get_type()){
			case TokenType::COMMA:
				if((bdepth == 0) && (pdepth == 0)){
					root.add_child(this->buildInfix(tokens, curr_start, i - 1, false));
					curr_start = i + 1;
				}
				break;
			case TokenType::LPAR:
				pdepth++;
				break;
            case TokenType::RPAR:
				pdepth--;
				if(pdepth < 0){
					throw ParserError(tokens.at(i));
					pdepth++;
				}
                break;
			case TokenType::LBRACK:
				bdepth++;
				break;
            case TokenType::RBRACK:
				bdepth--;
				if(pdepth < 0){
					throw ParserError(tokens.at(i));
					pdepth++;
				}
                break;
			default:
				break;
		}
	}
	root.add_child(this->buildInfix(tokens, curr_start, end, false));
	return root;
}
ASTree::ASNode ASTree::build_call(const std::vector<Token>& tokens, unsigned start, unsigned end, unsigned func_end){
	ASNode root{tokens.at(func_end)};
	if((start + 2 < func_end) && wrapped(tokens, start, func_end - 1)){
		root.add_child(this->buildInfix(tokens, start + 1, func_end - 2, true));
	}
	else{
		root.add_child(this->buildInfix(tokens, start, func_end - 1, false));
	}
	if(func_end + 1 == end){
		return root;
	}
	int curr_start{static_cast<int>(func_end + 1)};
	int bdepth{};
	int pdepth{};
	for(unsigned i{start + 2}; i < end; i++){
		switch(tokens.at(i).get_type()){
			case TokenType::COMMA:
				if((bdepth == 0) && (pdepth == 0)){
					root.add_child(this->buildInfix(tokens, curr_start, i - 1, false));
					curr_start = i + 1;
				}
				break;
			case TokenType::LPAR:
				pdepth++;
				break;
            case TokenType::RPAR:
				pdepth--;
				if(pdepth < 0){
					throw ParserError(tokens.at(i));
					pdepth++;
				}
                break;
			case TokenType::LBRACK:
				bdepth++;
				break;
            case TokenType::RBRACK:
				bdepth--;
				if(bdepth < 0){
					throw ParserError(tokens.at(i));
					bdepth++;
				}
                break;
			default:
				break;
		}
	}
	root.add_child(this->buildInfix(tokens, curr_start, end - 1, false));
	return root;
}
ASTree::ASNode ASTree::build_access(const std::vector<Token>& tokens, unsigned start, unsigned end, unsigned var_end){
	ASNode root{tokens.at(end)};
	if((start + 2 < var_end) && wrapped(tokens, start, var_end - 1)){
		root.add_child(this->buildInfix(tokens, start + 1, var_end - 2, true));
	}
	else{
		root.add_child(this->buildInfix(tokens, start, var_end - 1, false));
	}
	if(var_end + 1 >= end){
		throw ParserError(tokens.at(end));
	}
	if(wrapped(tokens, var_end + 1, end - 1)){
		root.add_child(this->buildInfix(tokens, var_end + 2, end - 2, true));
	}
	else{
		root.add_child(this->buildInfix(tokens, var_end + 1, end - 1, false));
	}
	return root;
}

ASTree::ASTree(const std::vector<Token>& tokens, unsigned start, unsigned end, bool infix) {
    if(tokens.empty()){
	    throw ParserError(Token{});
    }
    if(infix){
	    if(wrapped(tokens, start, end)){
	    this->proot = this->buildInfix(tokens, start + 1, end - 1, true);
	    }
	    else{
		    this->proot = this->buildInfix(tokens, start, end, false);
	    }
    }else{
    this->proot = this->build(tokens, 0, tokens.size() - 1);
    }
}
ASTree::ASTree(const std::vector<Token>& tokens, bool infix) : ASTree{tokens, 0, static_cast<unsigned>(tokens.size() - 1), infix} {}


std::vector<std::pair<int,int>> ASTree::get_child_idx(const std::vector<Token>& tokens, int start, int end){
    /*
    Takes a list of tokens assumed to be inside the parentheses of an operator and produce a list of start and end points based on locations of left and right parentheses
    this is used to determine the number of and location of child nodes in the list.
    for example given 2 (* 4 3 2) 3 
    would output [(0.0), (1,6), (7,7)]
    */
    int pdepth = 0;
    std::vector<std::pair<int,int>> child_idx{};
    TokenType parentT{tokens.at(start - 1).get_type()};
    int parStart=0;
    for(int i = start; i <= end; i++){
        Token curr = tokens.at(i);

        switch(curr.get_type()){
            case TokenType::VAR:
		if(pdepth == 0){
                
                child_idx.push_back(std::pair<int,int>(i,i));
		    }
		break;
            case TokenType::CONST:
		    if(parentT == TokenType::ASSIGN){
			if (child_idx.size() == 0){
				throw ParserError{curr};
			}
			else if((tokens.at((child_idx.back().first)).get_type() == TokenType::CONST) || (tokens.at((child_idx.back().first)).get_type() == TokenType::LPAR)){
				throw ParserError{curr};
			}
		}
		    if(pdepth == 0){
                
                child_idx.push_back(std::pair<int,int>(i,i));
		    }
                break;

            case TokenType::LPAR:
		if(parentT == TokenType::ASSIGN){
			if (child_idx.size() == 0){
				throw ParserError{curr};
			}
			else if((tokens.at((child_idx.back().first)).get_type() == TokenType::CONST) || (tokens.at((child_idx.back().first)).get_type() == TokenType::LPAR)){
				throw ParserError{curr};
			}
		}
                if(pdepth == 0){
                parStart = i;
                
                }
                pdepth++;
                break;
            case TokenType::RPAR:
                
                pdepth--;
                
                if(pdepth == 0){
                    child_idx.push_back(std::pair<int,int>(parStart,i));
                }
                else if(pdepth < 0){
                    //there's an extra outer parenthesi
                    //std::cout<<"THROW11"<<std::endl;
                    throw ParserError(tokens.at(i+1));
                }
                break;
            case TokenType::EXP:
            case TokenType::ASSIGN:
                if(i <= 0 || tokens.at(i - 1).get_type() != TokenType::LPAR){
                    //std::cout<<"THROW10"<<std::endl;
			        throw ParserError(curr);
            
		}
		break;
            case TokenType::ERR:
                //std::cout<<"THROW9"<<std::endl;
                throw ParserError(curr);
                break;
			default:
				throw ParserError(curr);
				break;
        }
        //currend++;
    }
	//multiple expression error
    if(child_idx.empty()){
	    throw ParserError(tokens.at(start));
    }
   
    return child_idx;
}

ASTree::ASNode ASTree::build(const std::vector<Token>& tokens, int start, int end){
	/* recursive builder for ASTrees, uses ASTree::get_child_idx() to find where children are located in the token vector and recursively build them out
		*/
    ASTree::ASNode curr{tokens[start]};
    ASTree::ASNode rootNode;
    
    std::vector<std::pair<int,int>> child_idx_list;

    switch(curr.get_pdata().get_type()){
        case TokenType::LPAR:
            //we know it should be an operand
            

            if((tokens[start+1].get_type() != TokenType::EXP) && tokens[start+1].get_type() != TokenType::ASSIGN){
                //std::cout<<"THROW9"<<std::endl;
                throw ParserError(tokens[start+1]);
            }
            else if(tokens[end].get_type() != TokenType::RPAR){
                //std::cout<<"THROW2"<<std::endl;
                throw ParserError(tokens[end], PErrType::END);
            }
            //create node for operand and ignore parentheses
            rootNode = tokens[start+1];
            //find where children begin and end
            child_idx_list  =this->get_child_idx(tokens, start+2, end-1);
            //recursively add children while properly building out their children
            for(const std::pair<int,int>& child_idx : child_idx_list){
                rootNode.add_child(build(tokens, child_idx.first, child_idx.second)); 
            }
	    if(tokens[start+1].get_type() == TokenType::ASSIGN){
                 for(unsigned i{}; i < child_idx_list.size() - 1; i++){
			 if(tokens.at(child_idx_list.at(i).first).get_type() == TokenType::LPAR){
				 throw ParserError(tokens.at(child_idx_list.at(i).first));
			 }
		 }
            }

            if((tokens[start+1].get_type() == TokenType::EXP && tokens[start+2].get_type() == TokenType::VAR) || tokens[start+1].get_type() == TokenType::ASSIGN){ //deals with (+ x), (= 3), (= x)
                std::vector<ASNode> kids{rootNode.get_kids()}; 
                if(kids.size() == 1){
                    throw ParserError(tokens[start+3]);
                }
            }
            
           
            if(tokens[start+1].get_type() == TokenType::ASSIGN){
                 std::vector<ASNode> kids{rootNode.get_kids()};
                
                if(kids.back().get_pdata().get_type() == TokenType::VAR){
                    //std::cout<<"THROW3"<<std::endl;
                    throw ParserError(kids.back().get_pdata());
                }
                
                for(unsigned i{}; i < kids.size() - 1; i++){
                    if(kids.at(i).get_pdata().get_type() != TokenType::VAR){
                        //std::cout<<"THROW4"<<std::endl;
                        throw ParserError(kids.at(i).get_pdata());
                    }
                }
            }
            return rootNode;
        case TokenType::VAR:
        case TokenType::CONST:
            //const has no children so can simply return
	    if(start == end){
            	return curr;
	    }
	    else{
            //std::cout<<"THROW5"<<std::endl;
		    throw ParserError(tokens.at(start+1));
	    }
	    break;
        default:
            //should not start with anything but CONST or LPAR
            //std::cout<<"THROW6"<<std::endl;
            throw ParserError(tokens[start]);
    }
}

ASTree::ASNode::ASNode(Token data) : pdata{data}, pchildren{} {}

ASTree::ASNode::ASNode(){ // default node constructor
    Token temp = Token(0,0,"",TokenType::ERR);
    pdata = temp;
    
}
void ASTree::ASNode::add_child(ASNode child) { this->pchildren.push_back(child); }

const ASTree::ASNode& ASTree::getProot()const {
    return this->proot;
}

const std::vector<ASTree::ASNode>& ASTree::ASNode::get_kids() const{
    return pchildren;
}

const Token& ASTree::ASNode::get_pdata() const{
    return pdata;
}

std::pair<unsigned, bool> ASTree::has_call(const std::vector<Token>& tokens, unsigned start, unsigned end){
	if((end < start + 2) || (tokens.at(end).get_type() != TokenType::RPAR)){
		return std::make_pair(0, false);
	}
	int pdepth{-1};
	for(int i{static_cast<int>(end - 1)}; i >= static_cast<int>(start); i--){
		switch(tokens.at(i).get_type()){
			case TokenType::LPAR:
				pdepth++;
				if(pdepth == 0){
					return std::make_pair(static_cast<unsigned>(i), true);
				}
				break;
			case TokenType::RPAR:
				pdepth--;
			default:
				break;
		}
	}
	return std::make_pair(0,false);
}
std::pair<unsigned, bool> ASTree::has_access(const std::vector<Token>& tokens, unsigned start, unsigned end){
	if((end < start + 2) || (tokens.at(end).get_type() != TokenType::RPAR)){
		return std::make_pair(0, false);
	}
	int pdepth{-1};
	for(int i{static_cast<int>(end - 1)}; i >= static_cast<int>(start); i--){
		switch(tokens.at(i).get_type()){
			case TokenType::LPAR:
				pdepth++;
				if(pdepth == 0){
					return std::make_pair(static_cast<unsigned>(i), true);
				}
				break;
			case TokenType::RPAR:
				pdepth--;
				break;
			default:
				break;
		}
	}
	return std::make_pair(0,false);
}
bool ASTree::is_callable(const std::vector<Token>& tokens, unsigned start, unsigned end){
	int pdepth{};
	int bdepth{};
	for(unsigned i{start}; i <= end; i++){
		switch(tokens.at(i).get_type()){
			case TokenType::LPAR:
				pdepth++;
				break;
			case TokenType::RPAR:
				pdepth--;
				if(pdepth < 0){
					throw ParserError(tokens.at(i));
				}
				break;
			case TokenType::LBRACK:
				bdepth++;
				break;
			case TokenType::RBRACK:
				bdepth--;
				if(pdepth < 0){
					throw ParserError(tokens.at(i));
				}
				break;
			case TokenType::CONST:
			case TokenType::BOOL:
			case TokenType::VOID:
			case TokenType::VAR:
				break;
			default:
				if((pdepth == 0) && (bdepth == 0)){
					return false;
				}
				break;
		}
	}
	if(!((pdepth == 0) && (bdepth == 0))){
		throw ParserError(tokens.at(end));
	}
	return true;
}
ASTree::ASNode ASTree::buildInfix(const std::vector<Token>& tokens, unsigned start, unsigned end, bool trimmed)
{
    if((start == end) && ((tokens[start].get_type() == TokenType::CONST) || (tokens[start].get_type() == TokenType::BOOL) || (tokens[start].get_type() == TokenType::VAR) || (tokens[start].get_type() == TokenType::VOID))){
	    return ASTree::ASNode{tokens[start]};
    }
	else if(wrapped_bracks(tokens, start, end)){
		return build_array(tokens, start, end - 1);
	}
	auto result{has_call(tokens, start, end)};
	if(result.second){
		if(is_callable(tokens, start, result.second)){
			return build_call(tokens, start, end, result.second);
		}
	}
	result = has_access(tokens, start, end);
	if(result.second){
		if(is_callable(tokens, start, result.second)){
			return build_access(tokens, start, end, result.second);
		}
	}
    
    int curr_pres{100};
    int low_idx{-1};
    int pdepth{};
	int bdepth{};
    ASTree::ASNode rootNode{};
    ASTree::ASNode right_child{};

    TokenType last{TokenType::EXP};

    for (unsigned i{start}; i <= end; i++)
    {
        const Token& curr = tokens[i];
        switch (curr.get_type()) {
            case TokenType::LPAR:
				if(((pdepth == 0) && (bdepth == 0))){
					last = TokenType::CONST;
				}
				pdepth++;
				break;
            case TokenType::RPAR:
				pdepth--;
				if(last == TokenType::EXP){
					throw ParserError(tokens.at(i));
				}
				if(pdepth < 0){
					throw ParserError(tokens.at(i));
					pdepth++;
				}
                break;
			case TokenType::LBRACK:
				if(((pdepth == 0) && (bdepth == 0))){
					last = TokenType::CONST;
					
				}
				bdepth++;
				break;
            case TokenType::RBRACK:
				bdepth--;
				if(last == TokenType::EXP){
					throw ParserError(tokens.at(i));
				}
				if(bdepth < 0){
					throw ParserError(tokens.at(i));
					bdepth++;
				}
                break;
			case TokenType::VAR:
			case TokenType::CONST:
			case TokenType::BOOL:
			case TokenType::VOID:
				if(((pdepth == 0) && (bdepth == 0))){
					
					if(last == TokenType::CONST){
						throw ParserError(curr);
					}
					
					last = TokenType::CONST;
					
				}
				break;
			case TokenType::ASSIGN: // WIP
				if(((pdepth == 0) && (bdepth == 0))){
					
					if(last == TokenType::EXP){
						throw ParserError(curr);
					}
					
					last = TokenType::EXP;
					
					if(curr_pres > precedence(curr.get_text())){
						if((i <= 0) || (tokens.at(i - 1).get_type() != TokenType::VAR)){
							throw ParserError(tokens.at(i));
						}
						curr_pres = precedence(curr.get_text());
						low_idx = i;
					}
				}
				break;
			case TokenType::LOG:
			case TokenType::EXP:
			case TokenType::EQUAL:
				if(((pdepth == 0) && (bdepth == 0))){
					if(last == TokenType::EXP){
						throw ParserError(curr);
					}
					
					last = TokenType::EXP;
					if(curr_pres >= precedence(curr.get_text())){
						if(i <= 0){
							throw ParserError(curr);
						}
						curr_pres = precedence(curr.get_text());
						low_idx = i;
					}
				}
				break;
            default:
				if((pdepth == 0) && (bdepth == 0)){
                	throw ParserError(curr);
				}
				break;
        }
    }

     if(low_idx == static_cast<int>(end) || (pdepth > 0)){
	     if(trimmed){
	    	throw ParserError(tokens.at(end+1));
	     }
	     else{
		throw ParserError(tokens.at(end), PErrType::END);
	     }
    }
    if(curr_pres == 100){
	    throw ParserError(tokens.at(end));
    }
    
    rootNode = ASTree::ASNode{tokens.at(low_idx)};
    if((low_idx > 1) && wrapped(tokens, start, low_idx - 1)){
    		rootNode.add_child(this->buildInfix(tokens, start + 1, low_idx - 2, true));
    }
    else if(low_idx > 0){
    	rootNode.add_child(this->buildInfix(tokens, start, low_idx - 1, false));
    }
    else{
	    throw ParserError(tokens.at(end));
    }
	
    if((end > 0) && wrapped(tokens, low_idx + 1, end)){  
    	rootNode.add_child(this->buildInfix(tokens, low_idx + 2, end - 1, true));
    }
    else{
	    rootNode.add_child(this->buildInfix(tokens, low_idx + 1, end, false));
    }
	
    return rootNode;
}

int ASTree::precedence(const std::string& text)
{
    switch(text[0]){
	    case '*':
	    case '/':
	    case '%':
	    	return 7;
	    case '+':
	    case '-':
	        return 6;
	    case '>':
	    case '<':
	    	return 5;
	    case '!':
	    case '=':
		if((text.size() > 1) && (text[1] == '=')){
		return 4;
		}
		else{
                return 0;
		}
	    	break;
	    case '&':
	    	return 3;
	    case '^':
	    	return 2;
	    case '|':
	    	return 1;
	    default:
	        return 100;
    }
}
