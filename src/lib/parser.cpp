#include "parser.h"
#include "error.h"
#include <iostream>

ASTree::ASTree(const std::vector<Token>& tokens) {
    auto infixedTokens = ASNode::infix_tokens(tokens);
    this->proot = this->build(infixedTokens, 0, infixedTokens.size() - 1);
}

// This function returns the precedence of the operators
int ASTree::ASNode::get_operator_precedence(TokenType type) {
    switch(type) {
        case TokenType::LPAR:
        case TokenType::RPAR: return 4;
        case TokenType::VAR:
        case TokenType::CONST: return 3;
        case TokenType::EXP: return 2;
        case TokenType::EQUAL: return 1;
        default: return 0;
    }
}

std::vector<Token> ASTree::ASNode::infix_tokens(const std::vector<Token>& tokens) {
    std::vector<Token> output;  // Vector to store the resulting ordered tokens.
    std::vector<Token> opstack; // Operator stack used for keeping track of operators.

    // Iterate over all the input tokens.
    for (const auto& token : tokens) {
        switch (token.get_type()) {
            // If token is a variable or constant, add it to the output.
            case TokenType::VAR:
            case TokenType::CONST:
                output.push_back(token);
                break;

            // If token is a left parenthesis, push it onto the opstack.
            case TokenType::LPAR:
                opstack.push_back(token);
                break;

            // If token is a right parenthesis, pop operators from opstack onto the output until a left parenthesis is encountered.
            case TokenType::RPAR:
                while (!opstack.empty() && opstack.back().get_type() != TokenType::LPAR) {
                    output.push_back(opstack.back());
                    opstack.pop_back();
                }
                // Pop the left parenthesis from the opstack.
                if (!opstack.empty()) opstack.pop_back();
                break;

            // If token is an expression or equal sign, 
            // pop operators from opstack to output if their precedence is higher or equal,
            // and then push the token onto the opstack.
            case TokenType::EXP:
            case TokenType::EQUAL:
                while (!opstack.empty() && get_operator_precedence(opstack.back().get_type()) >= get_operator_precedence(token.get_type())) {
                    output.push_back(opstack.back());
                    opstack.pop_back();
                }
                opstack.push_back(token);
                break;

            default:
                // Do nothing for other token types.
                break;
        }
    }

    // Pop any remaining operators from the opstack onto the output.
    while (!opstack.empty()) {
        output.push_back(opstack.back());
        opstack.pop_back();
    }

    return output;
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
    
    int parStart=0;
    for(int i = start; i <= end; i++){
        Token curr = tokens.at(i);

        switch(curr.get_type()){
            case TokenType::VAR:
            case TokenType::CONST:
		    if(pdepth == 0){
                child_idx.push_back(std::pair<int,int>(i,i));
		    }
                break;

            case TokenType::LPAR:
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
                    //there's an extra outer parenthesis
                    throw ParserError(tokens.at(i+1));
                }
                break;
            case TokenType::EXP:
            case TokenType::EQUAL:
                if(i <= 0 || tokens.at(i - 1).get_type() != TokenType::LPAR){
			throw ParserError(curr);
            
		}
		break;
            case TokenType::ERR:
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
            if((tokens[start+1].get_type() != TokenType::EXP) || (tokens[start+1].get_type() != TokenType::EQUAL)){
                throw ParserError(tokens[start+1]);
            }
            else if(tokens[end].get_type() != TokenType::RPAR){
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
            if(curr.get_pdata().get_type() == TokenType::EQUAL){
                std::vector<ASNode> kids{curr.get_kids()};
                if(kids.back().get_pdata().get_type() == TokenType::VAR){
                    throw ParserError(kids.back().get_pdata());
                }
                for(unsigned i{}; i < kids.size() - 1; i++){
                    if(kids.at(i).get_pdata().get_type() != TokenType::VAR){
                        throw ParserError(kids.back().get_pdata());
                    }
                }
            }
            return rootNode;
        case TokenType::CONST:
            //const has no children so can simply return
	    if(start == end){
            	return curr;
	    }
	    else{
		    throw ParserError(tokens.at(start+1));
	    }
	    break;
        default:
            //should not start with anything but CONST or LPAR
            throw ParserError(tokens[start]);
    }
}

ASTree::ASNode::ASNode(Token data) : pdata{data}, pchildren{} {}

ASTree::ASNode::ASNode(){ // default node constructor
    Token temp = Token(0,0,"",TokenType::ERR);
    pdata = temp;
    
}
void ASTree::ASNode::add_child(ASNode child) { this->pchildren.push_back(child); }

ASTree::ASNode ASTree::getProot(){
    return this->proot;
}

const std::vector<ASTree::ASNode>& ASTree::ASNode::get_kids() const{
    return pchildren;
}

/* double ASTree::calc(){

	return proot.calcHelp();
	
} */
/* void ASTree::print(){
    proot.printHelp();
}

void ASTree::ASNode::printHelp() const{ 

    switch(this->pdata.get_type()){

		case TokenType::EXP: //checks the token type - if it is an expression, more recursion needs to be done on the children of the expression
			std::cout << '(';
			for(size_t i =0; i < this->pchildren.size();i++){ //loops through all children of the current node being examined
				this->pchildren.at(i).printHelp(); //recursive call

				if(i == this->pchildren.size() - 1){
					std::cout << ')'; // if it is the last node in a child list, print a ')'
				}
				else{
					switch(this->pdata.get_text()[0]){ // if it isnt at the end of the list print the operator
				
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
		    std::cout<<std::stod(this->pdata.get_text());
            break;

        default:
		throw ParserError(this->pdata);
            break;

			
	}


}
 */

/* double ASTree::ASNode::calcHelp(){
    double ret = 0.0; // will be returned
	switch(this->pdata.get_type()){

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


} */


const Token& ASTree::ASNode::get_pdata() const{
    return pdata;
}
