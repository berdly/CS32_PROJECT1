#include "parser.h"
#include "error.h"
#include <iostream>
ASTree::ASTree(const std::vector<Token>& tokens) {
    std::vector<Token> infixTokens = infixWithParens(tokens);
    this->proot = this->build(infixTokens, 0, infixTokens.size() - 1);
}

// Convert an infix expression to a postfix expression
std::vector<Token> ASTree::infixToPostfix(const std::vector<Token>& tokens) {
    std::vector<Token> output;  // Holds the resulting postfix expression
    std::vector<Token> stack;   // Used to store operators and ensure proper order

    // Process each token in the infix expression
    for (const Token& token : tokens) {
        switch (token.get_type()) {
            case TokenType::CONST:
            case TokenType::VAR:
                // Directly add constants and variables to the output
                output.push_back(token);
                break;
            case TokenType::LPAR:
                // Push left parentheses onto the stack
                stack.push_back(token);
                break;
            case TokenType::RPAR:
                // Pop operators from the stack until a left parenthesis is encountered
                while (!stack.empty() && stack.back().get_type() != TokenType::LPAR) {
                    output.push_back(stack.back());
                    stack.pop_back();
                }
                // Ensure there's a matching left parenthesis for every right parenthesis
                if (stack.empty()) {
                    throw ParserError(token);  // If not, throw an error
                }
                stack.pop_back();  // Pop the left parenthesis from the stack
                break;
            case TokenType::EXP:
            case TokenType::EQUAL:
                // Ensure operators in the stack have higher precedence before pushing the current token
                while (!stack.empty() && getPrecedence(stack.back()) >= getPrecedence(token)) {
                    output.push_back(stack.back());
                    stack.pop_back();
                }
                stack.push_back(token);
                break;
            default:
                // If an unexpected token is encountered, throw an error
                throw ParserError(token);
        }
    }

    // Pop any remaining operators from the stack and add them to the output
    while (!stack.empty()) {
        if (stack.back().get_type() == TokenType::LPAR) {
            throw ParserError(stack.back(), PErrType::END); // Error for unmatched left parenthesis
        }
        output.push_back(stack.back());
        stack.pop_back();
    }

    return output;
}

// Get the precedence level of an operator
int ASTree::getPrecedence(const Token& token) {
    switch (token.get_type()) {
        case TokenType::EXP:
            if (token.get_text() == "*" || token.get_text() == "/") {
                return 3;
            } else if (token.get_text() == "+" || token.get_text() == "-") {
                return 2;
            }
            break;
        case TokenType::EQUAL:
            return 1;
        default:
            // If an unknown token is encountered when checking precedence, throw an error
            throw LexerError(token);
    }
    return -1;
}

// Convert a postfix expression back to infix
std::vector<Token> ASTree::postfixToInfix(const std::vector<Token>& postfixTokens) {
    std::vector<std::vector<Token>> stack;  // Used to hold intermediate results

    // Process each token in the postfix expression
    for (const Token& token : postfixTokens) {
        if (token.get_type() == TokenType::CONST || token.get_type() == TokenType::VAR) {
            // Directly push constants and variables onto the stack
            stack.push_back({token});
        } else {
            // Ensure there are enough operands on the stack for the current operator
            if (stack.size() < 2) {
                throw ParserError(token);
            }
            std::vector<Token> right = stack.back(); stack.pop_back();
            std::vector<Token> left = stack.back(); stack.pop_back();
            // Create an infix sub-expression from the two operands and the operator
            std::vector<Token> combined = { Token(0, 0, "(", TokenType::LPAR) };
            combined.insert(combined.end(), left.begin(), left.end());
            combined.push_back(token);
            combined.insert(combined.end(), right.begin(), right.end());
            combined.push_back(Token(0, 0, ")", TokenType::RPAR));
            stack.push_back(combined);
        }
    }

    // Ensure the postfix expression was valid
    if (stack.size() != 1) {
        throw ParserError(Token(0, 0, "", TokenType::ERR);
    }

    return stack.back();
}

// Convert an infix expression to postfix and then back to infix with proper parentheses
std::vector<Token> ASTree::infixWithParens(const std::vector<Token>& tokens) {
    std::vector<Token> postfix = infixToPostfix(tokens);
    return postfixToInfix(postfix);
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

const ASTree::ASNode& ASTree::getProot()const {
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
