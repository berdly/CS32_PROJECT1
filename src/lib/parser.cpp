#include "parser.h"
#include "error.h"
#include <iostream>
ASTree::ASTree(const std::vector<Token>& tokens) {
    this->proot = this->buildInfix(tokens);
}


ASTree::ASNode ASTree::buildInfix(const std::vector<Token>& tokens)
{
    /* Todo
        Work on parser error, if an operand doesn't have a left const or right const or parentheses, then throw the error 
        Work on not balanced parentheses, error. 
        ....
    */
    std::stack<Token> t_stack;
    std::stack<ASNode> n_stack;
    std::stack<ASNode> swap;

    ASTree::ASNode rootNode;
    
    for (int i = 0; i < tokens.size(); i++)
    {
        Token temp = tokens[i];

        switch (temp.get_type()) {
            case TokenType::LPAR:
                t_stack.push(temp);
                break;
            case TokenType::RPAR:
                while (!t_stack.empty() && t_stack.top().get_type() != TokenType::LPAR)
                {
                    rootNode = t_stack.top();
                    t_stack.pop();
                    swap.push(n_stack.top());
                    n_stack.pop();
                    rootNode.add_child(n_stack.top());
                    n_stack.pop();
                    rootNode.add_child(swap.top());
                    swap.pop();
                    n_stack.push(rootNode);
                }

                t_stack.pop();
                break;
            //case TokenType::VAR: // WIP
            case TokenType::CONST:
                n_stack.push(ASTree::ASNode{temp});
                break;
            //case TokenType::EQUAL: // WIP
            case TokenType::EXP:
                //if (precendense(temp.get_text()) > 0)
                //{
                    while ((!t_stack.empty() && t_stack.top().get_type() != TokenType::LPAR) && (precendense(t_stack.top().get_text()) >= precendense(temp.get_text())))
                    {
                        rootNode = t_stack.top();
                        t_stack.pop();
                        swap.push(n_stack.top());
                        n_stack.pop();
                        rootNode.add_child(n_stack.top());
                        n_stack.pop();
                        rootNode.add_child(swap.top());
                        swap.pop();
                        n_stack.push(rootNode);
                    }
                
                    t_stack.push(temp);
                    break;
                //}
            default:
                throw ParserError(temp);
        }
    }

    while (!t_stack.empty())
    {
        rootNode = t_stack.top();
        t_stack.pop();
        swap.push(n_stack.top());
        n_stack.pop();
        rootNode.add_child(n_stack.top());
        n_stack.pop();
        rootNode.add_child(swap.top());
        swap.pop();
        n_stack.push(rootNode);
    }

    rootNode = n_stack.top();
    n_stack.pop();
    return rootNode;
}

double ASTree::precendense(std::string text)
{
    double pVal;

    if (text == "*" || text == "/")
    {
        pVal = 3;
    }
    else if (text == "+" || text == "-")
    {
        pVal = 2;
    }
    else if (text == "=")
    {
        pVal = 1;
    }
    else
    {
        pVal = 0;
    }

    return pVal;
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
