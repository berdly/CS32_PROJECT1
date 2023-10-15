#include "parser.h"
#include "error.h"
#include <iostream>

ASTree::ASTree(const std::vector<Token>& tokens) {
    this->proot = this->build(tokens, 0, tokens.size() - 1);
}

std::vector<std::pair<int,int>> ASTree::get_child_idx(const std::vector<Token>& tokens, int start, int end){
    /*TODO
    should take a list of tokens assumed to be inside the parentheses of an operator and produce a list of start and end points based on locations of left and right parentheses
    this is used to determine the number of and location of child nodes in the list.
    for example given 2 (* 4 3 2) 3 
    would output [(0.0), (1,6), (7,7)]
    */
    int pdepth{};
    std::vector<std::pair<int,int>> child_idx{};
    int currstart{start};
    int currend{end};
    for(const Token& token : tokens){
        switch(token.get_type()){
            case TokenType::CONST:
		if(pdepth == 0){
                child_idx.push_back(std::pair<int,int>(currstart,currstart));
                currstart++;
		}
                break;
            case TokenType::LPAR:
                pdepth++;
                break;
            case TokenType::RPAR:
                pdepth--;
                if(pdepth == 0){
                    child_idx.push_back(std::pair<int,int>(currstart,currend));
                    currstart = currend + 1;
                }
                else if(pdepth < 0){
                    throw ParserError(token);
                }
                break;
            case TokenType::EXP:
                break;
            case TokenType::ERR:
                throw ParserError(token);
                break;
        }
        currend++;
    }
    return child_idx;
}

ASTree::ASNode ASTree::build(const std::vector<Token>& tokens, int start, int end){
    ASTree::ASNode curr{tokens[start]};
    ASTree::ASNode rootNode;
    
    std::vector<std::pair<int,int>> child_idx_list;
    switch(curr.get_pdata().get_type()){
        case TokenType::LPAR:
            //we know it should be an operand
            if(tokens[start+1].get_type() != TokenType::EXP){
                throw ParserError(tokens[start+1]);
            }
            else if(tokens[end].get_type() != TokenType::RPAR){
                throw ParserError(tokens[end]);
            }
            //create node for operand and ignore parentheses
            rootNode = tokens[start+1];
            //find where children begin and end
            child_idx_list  =this->get_child_idx(tokens, start+2, end-1);
            //recursively add children while properly building out their children
            for(const std::pair<int,int>& child_idx : child_idx_list){
                rootNode.add_child(build(tokens, child_idx.first, child_idx.second));
                }
            return rootNode;
        case TokenType::CONST:
            //const has no children so can simply return
            return curr;
        default:
            //should not start with anything but CONST or LPAR
            throw ParserError(tokens[start]);
    }   
}

ASTree::ASNode::ASNode(Token data) : pdata{data}, pchildren{} {}

void ASTree::ASNode::add_child(ASNode child) { this->pchildren.push_back(child); }


double ASTree::calc(){

	
	return proot.calcHelp();
	

}
void ASTree::print(){
    proot.printHelp();
}

void ASTree::ASNode::printHelp(){

    switch(this->pdata.get_type()){

		case TokenType::EXP:
			std::cout << '(';
			for(size_t i =0; i < this->pchildren.size();i++){
				this->pchildren.at(i).printHelp();

				if(i == this->pchildren.size() - 1){
					std::cout << ')';
				}
				else{
					switch(this->pdata.get_text()[0]){
				
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

		
	    case TokenType::CONST:
		    std::cout<<this->pdata.get_text();
            break;
        case TokenType::LPAR:
            break;
        case TokenType::RPAR:
            break;
        case TokenType::ERR:
            break;

        default:
            break;

			
	}


}


double ASTree::ASNode::calcHelp(){
    double ret = 0.0; // will be returned
	switch(this->pdata.get_type()){
		
		case TokenType::EXP:
			
			for(size_t i =0; i < this->pchildren.size();i++){
				double val = pchildren.at(i).calcHelp();

				if(i ==0){
					ret = val;
				}else{
					switch(this->pdata.get_text()[0]){
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
				return ret;
			}
            break;
	    case TokenType::CONST:
		    return std::stod(this->pdata.get_text());
            break;
        case TokenType::LPAR:
            return -1;
            break;
        case TokenType::RPAR:
            return -1;
            break;
        case TokenType::ERR:
            return -1;
            break;
        default:
            return -1;
            break;

		
	}


}


Token ASTree::ASNode::get_pdata(){
    return pdata;
}
