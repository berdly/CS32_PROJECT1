#include "lexer.h"
#include <cstdlib>
#include <cctype>
#include <sstream>

std::vector<std::vector<Token>> split(const std::vector<Token>& input){
    int pdepth = 0;
    std::vector<std::vector<Token>> statements{};
    int parStart=0;
    for(unsigned i{}; i < input.size(); i++){
        Token curr = input.at(i);

        switch(curr.get_type()){
            case TokenType::VAR:
            case TokenType::CONST:
		        if(pdepth == 0){
                    statements.push_back(std::vector<Token>{input.at(i)});
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
                    statements.emplace_back(input.begin() + parStart, input.begin() + i + 1);
                }
                else if(pdepth < 0){
                    //there's an extra outer parentheses, current behavior is add it to last statement, can add arbitrary number to end of statement
                    statements.back().push_back(input.at(i));
                    pdepth = 0;
                }
                break;
            case TokenType::EXP:
            case TokenType::EQUAL:
		if((i == 0) || (input.at(i - 1).get_type()) != TokenType::LPAR){
			if(pdepth == 0){
				parStart = i;
			}
			pdepth++;
		}
		break;
            case TokenType::ERR:
                //std::cout<<"THROW9"<<std::endl;
                break;
        }
        if((i == (input.size() - 1)) && pdepth > 0){
            statements.emplace_back(input.begin() + parStart, input.begin() + i + 1);
        }
    }
    return statements;
}
// Function to parse the input string and create tokens for each number and operand.
std::vector<Token> reader(const std::string& input, bool err) {  // Change return type to vector
    std::vector<Token> tokens;  // Use vector instead of stack to store generated tokens
    bool startsNum{false};
    bool startsVar{false};
    bool startsPer{false};
    // Temporary string to accumulate digits of a number.
    std::string currToken;
    int column = 1;
    int line = 1;

    for (size_t i = 0; i < input.size(); ++i) {
        char ch = input[i];
        
        
        // Switch-case to handle different characters.
        switch (ch) {
            // For arithmetic operators.
            case '+':
            case '-':
            case '*':
            case '/':
                // If there's an accumulated number, create a token for it.
                if(startsPer){
                   std::cout << "Syntax error on line " << line << " column " << column << "." << std::endl;
                   currToken.clear();
		   if(err){
                   exit(1);
		   }
		   else{
			   return std::vector<Token>{Token{0,0,"",TokenType::ERR}};
		   }
                }                
                if (!currToken.empty()) {
                if(startsVar){
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::VAR));
                    currToken.clear();
                    startsVar = false;
                }
                else if(startsNum){
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::CONST));
                    currToken.clear();
                    startsNum = false;
                }
            }
                // Create a token for the operator.
                tokens.push_back(Token(column, line, std::string(1, ch), TokenType::EXP));
                break;

            // For left parenthesis.
            case '(':
                // If there's an accumulated number, create a token for it.
                if (!currToken.empty()) {
                if(startsVar){
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::VAR));
                    currToken.clear();
                    startsVar = false;
                }
                else if(startsNum){
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::CONST));
                    currToken.clear();
                    startsNum = false;
                }
            }
                // Create a token for the left parenthesis.
                tokens.push_back(Token(column, line, std::string(1, ch), TokenType::LPAR));
                break;

            // For right parenthesis.
            case ')':
                // If there's an accumulated number, create a token for it.
                 if (!currToken.empty()) {
                if(startsVar){
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::VAR));
                    currToken.clear();
                    startsVar = false;
                }
                else if(startsNum){
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::CONST));
                    currToken.clear();
                    startsNum = false;
                }
            }
                // Create a token for the right parenthesis.
                tokens.push_back(Token(column, line, std::string(1, ch), TokenType::RPAR));
                break;
            case '=':
                // If there's an accumulated number, create a token for it.
                if (!currToken.empty()) {
                if(startsVar){
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::VAR));
                    currToken.clear();
                    startsVar = false;
                }
                else if(startsNum){
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::CONST));
                    currToken.clear();
                    startsNum = false;
                }
            }
                // Create a token for the left parenthesis.
                tokens.push_back(Token(column, line, std::string(1, ch), TokenType::EQUAL));
                break;
            // For decimal points.
            case '.':
                // Check for multiple decimal points in the current number.
                startsPer = true;
                if(startsVar){
                   std::cout << "Syntax error on line " << line << " column " << column << "." << std::endl;
                   currToken.clear();
		   if(err){
                   exit(1);
		   }
		   else{
			   return std::vector<Token>{Token{0,0,"",TokenType::ERR}};
		   }
                }
                if (currToken.find('.') != std::string::npos) {
                   std::cout << "Syntax error on line " << line << " column " << column << "." << std::endl;
                   currToken.clear();
		   if(err){
                   exit(1);
		   }
		   else{
			   return std::vector<Token>{Token{0,0,"",TokenType::ERR}};
		   }
                }
                // Check for trailing decimals.
                else if (i + 1 == input.size() || isspace(input[i + 1]) || input[i + 1] == '\n') {
                    std::cout << "Syntax error on line " << line << " column " << column + 1 << "." << std::endl;
                    currToken.clear();
                    if(err){
                   exit(1);
		   }
		   else{
			   return std::vector<Token>{Token{0,0,"",TokenType::ERR}};
		   }
               }
                // Check for leading decimals.
               else if (currToken.empty()) {
                   std::cout << "Syntax error on line " << line << " column " << column << "." << std::endl;
                   currToken.clear();
                   if(err){
                   exit(1);
		   }
		   else{
			   return std::vector<Token>{Token{0,0,"",TokenType::ERR}};
		   }
               } else {
                   currToken += ch;
                }
               break;
            
            // For newline characters.
            case '\n':
            // If there's an accumulated number, create a token for it.
            if (!currToken.empty()) {
                if(startsVar){
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::VAR));
                    currToken.clear();
                    startsVar = false;
                }
                else if(startsNum){
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::CONST));
                    currToken.clear();
                    startsNum = false;
                }
            }
                line++;
                column = 0;
                break;

            case ' ':
                // White space seperate
               
                if (!currToken.empty()) {
                if(startsVar){
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::VAR));
                    currToken.clear();
                    startsVar = false;
                }
                else if(startsNum){
                    
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::CONST));
                    currToken.clear();
                    startsNum = false;
                }
            }
                break;               
            
            // For other characters.
            default:
                // If it's a digit, add to the current number.
                if (isdigit(ch)) {
                    if(!(startsNum || startsVar)){
                        startsNum = true;
                        startsPer = false;
                    }
                    currToken += ch;
                } 
                // If it's an invalid character, create an error token.
                else if(isalpha(ch) || ch == '_'){
                    if(!(startsNum || startsVar)){
                        startsVar = true;
                    }
                    if(startsNum){
                        std::cout<<"Syntax error on line "<< line <<" column " << column << "." << std::endl;
                        currToken.clear();
                        if(err){
                   	exit(1);
		   }
		   else{
			   return std::vector<Token>{Token{0,0,"",TokenType::ERR}};
		   }
                    }
                    currToken += ch;
                }
                else if (!isspace(ch)) {
                    std::cout<<"Syntax error on line "<< line <<" column " << column << "." << std::endl;
                    currToken.clear();
                    if(err){
		    exit(1);
		   }
		   else{
			   return std::vector<Token>{Token{0,0,"",TokenType::ERR}};
		   }
                }

                if(isspace(ch)){
                    if (!currToken.empty()) {
                if(startsVar){
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::VAR));
                    currToken.clear();
                    startsVar = false;
                }
                else if(startsNum){
                    
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::CONST));
                    currToken.clear();
                    startsNum = false;
                }
            }

                }

                break;
        }
        column++;
    }

    // If there's any remaining number, create a token for it.
    if (!currToken.empty()) {
        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::CONST));
    }
    return tokens;
}
