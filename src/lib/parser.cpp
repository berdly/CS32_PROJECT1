

#include "parser.h"
#include "error.h"
#include <stack>
#include <iostream>
#include <stdexcept>
bool wrapped(const std::vector<Token>& tokens, unsigned start, unsigned end){
	int pdepth{};
	if(tokens.at(start).get_type() == TokenType::LPAR){
		for(unsigned i{start+ 1}; i <= end; i++){
			if(tokens.at(i).get_type() == TokenType::LPAR){
				pdepth++;
			}
			else if(tokens.at(i).get_type() == TokenType::RPAR){
				if(pdepth == 0){
					return (i == tokens.size() - 1);
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
ASTree::ASTree(const std::vector<Token>& tokens, bool infix) {
    if(tokens.empty()){
	    throw ParserError(Token{});
    }
    if(infix){
	    if(wrapped(tokens, 0, tokens.size() - 1)){
	    this->proot = this->buildInfix(tokens, 1, tokens.size() - 2);
	    }
	    else{
		    this->proot = this->buildInfix(tokens, 0, tokens.size() - 1);
	    }
    }else{
    this->proot = this->build(tokens, 0, tokens.size() - 1);
    }
}


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
		    if(parentT == TokenType::EQUAL){
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
		if(parentT == TokenType::EQUAL){
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
            case TokenType::EQUAL:
                if(i <= 0 || tokens.at(i - 1).get_type() != TokenType::LPAR){
                    //std::cout<<"THROW10"<<std::endl;
			        throw ParserError(curr);
            
		}
		break;
            case TokenType::ERR:
                //std::cout<<"THROW9"<<std::endl;
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
            

            if(tokens[start+1].get_type() != TokenType::EXP and tokens[start+1].get_type() != TokenType::EQUAL){
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
	    if(tokens[start+1].get_type() == TokenType::EQUAL){
                 for(unsigned i{}; i < child_idx_list.size() - 1; i++){
			 if(tokens.at(child_idx_list.at(i).first).get_type() == TokenType::LPAR){
				 throw ParserError(tokens.at(child_idx_list.at(i).first));
			 }
		 }
            }

            if((tokens[start+1].get_type() == TokenType::EXP and tokens[start+2].get_type() == TokenType::VAR) or tokens[start+1].get_type() == TokenType::EQUAL){ //deals with (+ x), (= 3), (= x)
                std::vector<ASNode> kids{rootNode.get_kids()}; 
                if(kids.size() == 1){
                    throw ParserError(tokens[start+3]);
                }
            }
            
           
            if(tokens[start+1].get_type() == TokenType::EQUAL){
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

ASTree::ASNode ASTree::buildInfix(const std::vector<Token>& tokens, unsigned start, unsigned end)
{
    if((start == end) && ((tokens[start].get_type() == TokenType::CONST) || (tokens[start].get_type() == TokenType::VAR))){
	    return ASTree::ASNode{tokens[start]};
    }
    
    int curr_pres{100};
    int low_idx{};
    int pdepth{};
    ASTree::ASNode rootNode{};
    ASTree::ASNode right_child{};

    TokenType last{TokenType::EXP};

    for (unsigned i{start}; i <= end; i++)
    {
        const Token& curr = tokens[i];

        switch (curr.get_type()) {
            case TokenType::LPAR:
		if(pdepth == 0){
			
			if(last == TokenType::CONST){
				throw ParserError(curr);
			}
			last = TokenType::CONST;
			
		}
		pdepth++;
		break;
            case TokenType::RPAR:
  		pdepth--;
		if(pdepth < 0){
			throw ParserError(tokens.at(i), PErrType::END);
		}
                break;
            case TokenType::VAR:
            case TokenType::CONST:
		if(pdepth == 0){
			
			if(last == TokenType::CONST){
				throw ParserError(curr);
			}
			
			last = TokenType::CONST;
   			
		}
		break;
            case TokenType::EQUAL: // WIP
		if(pdepth == 0){
			
			if(last == TokenType::EXP){
				throw ParserError(curr);
			}
			
			last = TokenType::EXP;
   			
			if(curr_pres > precedence(curr.get_text())){
				if(!((i > 0) || (tokens.at(i - 1).get_type() == TokenType::VAR))){
					throw ParserError(tokens.at(i));
				}
				curr_pres = precedence(curr.get_text());
				low_idx = i;
			}
		}
                break;
            case TokenType::EXP:
                if((pdepth == 0) && (curr_pres >= precedence(curr.get_text()))){
			
			if(last == TokenType::EXP){
				throw ParserError(curr);
			}
			
			last = TokenType::EXP;
			
			if(i <= 0){
				throw ParserError(curr);
			}
			curr_pres = precedence(curr.get_text());
			low_idx = i;
		}
		break;
            default:
                throw ParserError(curr);
		break;
        }
    }

    if(curr_pres == 100){
	    throw ParserError(tokens.at(end));
    }
	    
    rootNode = ASTree::ASNode{tokens.at(low_idx)};
    if((tokens.at(start).get_type() == TokenType::LPAR && tokens.at(low_idx - 1).get_type() == TokenType::RPAR) && (low_idx > 1)){
    		rootNode.add_child(this->buildInfix(tokens, start + 1, low_idx - 2));
    }
    else if(low_idx > 0){
    	rootNode.add_child(this->buildInfix(tokens, start, low_idx - 1));
    }
    else{
	    throw ParserError(tokens.at(end));
    }
	
    if((tokens.at(low_idx + 1).get_type() == TokenType::LPAR) && (tokens.at(end).get_type() == TokenType::RPAR) && (end > 0)){  
    	rootNode.add_child(this->buildInfix(tokens, low_idx + 2, end - 1));
    }
    else{
	    rootNode.add_child(this->buildInfix(tokens, low_idx + 1, end));
    }
	
    return rootNode;
}

double ASTree::precedence(std::string text)
{
    switch(text[0]){
	    case '*':
	    case '/':
	    	return 3;
	    case '+':
	    case '-':
	        return 2;
	    case '=':
                return 1;
	    default:
	        return 100;
    }
}
