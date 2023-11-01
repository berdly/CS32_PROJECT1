#include "lexer.h"
#include <cstdlib>
#include <cctype>
#include <sstream>
#include "error.h"

std::vector<std::vector<Token>> split(const std::vector<Token>& input, unsigned start, unsigned end){
    int pdepth = 0;
    std::vector<std::vector<Token>> statements{};
    unsigned parStart{start};
    for(unsigned i{start}; i <= end; i++){
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
            case TokenType::ASSIGN:
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
        default:
            break;
        }
        if((i == (input.size() - 1)) && pdepth > 0){
            statements.emplace_back(input.begin() + parStart, input.begin() + i + 1);
        }
    }
    return statements;
}

std::vector<std::vector<Token>> split_infix(const std::vector<Token>& input, unsigned start, unsigned end) {
    std::vector<std::vector<Token>> statements{};
    /*int bdepth{};
    int pdepth{};
    unsigned curr_start{start};
    bool in_statement{false};
    TokenType last{TokenType::EXP};
    for(unsigned i{start}; i <= end; i++){
        switch(input.at(i).get_type()){
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::ELSE:
                if(!in_statement){
                    if(curr_start != i){
                        statements.emplace_back(input.begin() + curr_start, input.begin() + i);
                        curr_start = i;
                    }
                    pdepth = 0;
                    in_statement = true;
                    last = TokenType::EXP;
                }
                break;
            case TokenType::PRINT:
                if(!in_statement){
                    if(curr_start != i){
                        statements.emplace_back(input.begin() + curr_start, input.begin() + i);
                        curr_start = i;
                    }
                    pdepth = 0;
                    last = TokenType::EXP;
                }
                break;
            case TokenType::CONST:
            case TokenType::BOOL:
            case TokenType::VAR:
                if((!in_statement) && pdepth == 0 && last == TokenType::CONST){
                    statements.emplace_back(input.begin() + curr_start, input.begin() + i);
                    curr_start = i;
                    last = TokenType::EXP;
                }
                if(pdepth == 0){
                    last = TokenType::CONST;
                }
                break;
            case TokenType::LPAR:
                if((!in_statement) && pdepth == 0 && last == TokenType::CONST){
                    statements.emplace_back(input.begin() + curr_start, input.begin() + i);
                    curr_start = i;
                    last = TokenType::EXP;
                }
                pdepth++;
                break;
            case TokenType::RPAR:
                pdepth--;
                if(pdepth == 0){
                    statements.emplace_back(input.begin() + curr_start, input.begin() + i + 1);
                }
                else if(pdepth < 0){
                    //there's an extra outer parentheses, current behavior is add it to last statement, can add arbitrary number to end of statement
                    statements.back().push_back(input.at(i));
                    pdepth = 0;
                }
                break;
            case TokenType::EXP:
            case TokenType::LOG:
            case TokenType::EQUAL:
            case TokenType::ASSIGN:
                if((!in_statement) && pdepth == 0 && last == TokenType::EXP){
                    statements.emplace_back(input.begin() + curr_start, input.begin() + i + 1);
                    curr_start = i;
                }
                if(pdepth == 0){
                    last = TokenType::EXP;
                }
                break;
            case TokenType::LBRACE:
                if(in_statement){
                    bdepth++;
                }
                break;
            case TokenType::RBRACE:
                if(in_statement){
                    bdepth--;
                    if(bdepth == 0){
                        statements.emplace_back(input.begin() + curr_start, input.begin() + i + 1);
                        curr_start = i + 1;
                    }
                    in_statement = false;
                }
                break;
            default:
                throw ParserError(input.at(i));
        }  
    }
    */
    int curr_line{input.front().get_line()};
    bool in_statement{false};
    int bdepth{};
    bool in_block{false};
    int curr_start{};
    for(unsigned i{start}; i <= end; i++){
        if((!in_statement) && (input.at(i).get_line() != curr_line)){
            curr_line = input.at(i).get_line();
            statements.emplace_back(input.begin() + curr_start, input.begin() + i);
            curr_start = i;
        }
        switch(input.at(i).get_type()){
            case TokenType::WHILE:
            case TokenType::IF:
            case TokenType::ELSE:
                if(!in_statement){
                    in_statement = true;
                }
                break;
            case TokenType::LBRACE:
                if(in_statement){
                    bdepth++;
                    if(!in_block){
                        in_block = true;
                    }
                }
                break;
            case TokenType::RBRACE:
                if(in_statement){
                    bdepth--;
                    if(in_block && bdepth == 0){
                        in_block = false;
                        in_statement = false;
                    }
                }
                if(bdepth < 0){
                    statements.back().push_back(input.at(i));
                    bdepth = 0;
                }
                break;
            default:
                break;
        }
    }
    statements.emplace_back(input.begin() + curr_start, input.end());
    return statements;
}

// Function to parse the input string and create tokens for each number and operand.
std::vector<Token> reader(const std::string& input, bool err) {  // Change return type to vector
    std::vector<Token> tokens;  // Use vector instead of stack to store generated tokens
    bool startsNum{false};
    bool startsVar{false};
    bool isPer{false};
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
            case '%':
            
                // If there's an accumulated number, create a token for it.
                if (!currToken.empty()) {
                if(startsVar){
                    if((currToken == "while")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::WHILE));
                    }else if((currToken == "if")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::IF));
                    }else if((currToken == "else")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::ELSE));
                    }else if((currToken == "print")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::PRINT));
                    }else if((currToken == "true") || (currToken == "false")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::BOOL));
                    }else{
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::VAR));
                    }
                    currToken.clear();
                    startsVar = false;
                }
                else if(startsNum){
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::CONST));
                    currToken.clear();
                    startsNum = false;
                }
                if(isPer){ //error for periods in opperator cases
                   std::cout << "Syntax error on line " << line << " column " << column << "." << std::endl;
                   currToken.clear();
                   if(err){
                   exit(1);
		   }
		   else{
			   return std::vector<Token>{Token{0,0,"",TokenType::ERR}};
		   }                    
                }                
            }
                // Create a token for the operator.
                tokens.push_back(Token(column, line, std::string(1, ch), TokenType::EXP));
                break;
            case '<':
            case '>':

                 // If there's an accumulated number, create a token for it.
                if (!currToken.empty()) {
                if(startsVar){
                    if((currToken == "while")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::WHILE));
                    }else if((currToken == "if")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::IF));
                    }else if((currToken == "else")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::ELSE));
                    }else if((currToken == "print")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::PRINT));
                    }else if((currToken == "true") || (currToken == "false")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::BOOL));
                    }else{
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::VAR));
                    }
                    currToken.clear();
                    startsVar = false;
                }
                else if(startsNum){
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::CONST));
                    currToken.clear();
                    startsNum = false;
                }
                if(isPer){ //error for periods in opperator cases
                   std::cout << "Syntax error on line " << line << " column " << column << "." << std::endl;
                   currToken.clear();
                   if(err){
                   exit(1);
		   }
		   else{
			   return std::vector<Token>{Token{0,0,"",TokenType::ERR}};
		   }                    
                }                
            }
                // Create a token for the operator.
            if(i != input.size()-1){
                if(input[i+1] == '='){
                tokens.push_back(Token(column, line, std::string(1, ch) + std::string(1,input[i+1]), TokenType::EXP));
                i++;
                column++;
                break;
                }

            }
            tokens.push_back(Token(column, line, std::string(1, ch), TokenType::EXP));
            break;

           
            case '&':
            case '|':
            case '^':
                if (!currToken.empty()) {
                if(startsVar){
                    if((currToken == "while")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::WHILE));
                    }else if((currToken == "if")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::IF));
                    }else if((currToken == "else")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::ELSE));
                    }else if((currToken == "print")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::PRINT));
                    }else if((currToken == "true") || (currToken == "false")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::BOOL));
                    }else{
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::VAR));
                    }
                    currToken.clear();
                    startsVar = false;
                }
                else if(startsNum){
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::CONST));
                    currToken.clear();
                    startsNum = false;
                }
                if(isPer){ //error for periods in opperator cases
                   std::cout << "Syntax error on line " << line << " column " << column << "." << std::endl;
                   currToken.clear();
                   if(err){
                   exit(1);
		   }
		   else{
			   return std::vector<Token>{Token{0,0,"",TokenType::ERR}};
		   }                    
                }                
            }
                // Create a token for the operator.
                tokens.push_back(Token(column, line, std::string(1, ch), TokenType::LOG));
                break;


             // For left parenthesis.
            case '(':
                // If there's an accumulated number, create a token for it.
                if (!currToken.empty()) {
                if(startsVar){
                    if((currToken == "while")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::WHILE));
                    }else if((currToken == "if")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::IF));
                    }else if((currToken == "else")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::ELSE));
                    }else if((currToken == "print")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::PRINT));
                    }else if((currToken == "true") || (currToken == "false")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::BOOL));
                    }else{
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::VAR));
                    }
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
                    if((currToken == "while")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::WHILE));
                    }else if((currToken == "if")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::IF));
                    }else if((currToken == "else")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::ELSE));
                    }else if((currToken == "print")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::PRINT));
                    }else if((currToken == "true") || (currToken == "false")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::BOOL));
                    }else{
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::VAR));
                    }
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

            case '{':

                if (!currToken.empty()) {
                if(startsVar){
                    if((currToken == "while")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::WHILE));
                    }else if((currToken == "if")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::IF));
                    }else if((currToken == "else")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::ELSE));
                    }else if((currToken == "print")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::PRINT));
                    }else if((currToken == "true") || (currToken == "false")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::BOOL));
                    }else{
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::VAR));
                    }
                    currToken.clear();
                    startsVar = false;
                }
                else if(startsNum){
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::CONST));
                    currToken.clear();
                    startsNum = false;
                }
            }
                tokens.push_back(Token(column, line, std::string(1, ch), TokenType::LBRACE));
                break;

            case '}':

                if (!currToken.empty()) {
                if(startsVar){
                    if((currToken == "while")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::WHILE));
                    }else if((currToken == "if")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::IF));
                    }else if((currToken == "else")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::ELSE));
                    }else if((currToken == "print")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::PRINT));
                    }else if((currToken == "true") || (currToken == "false")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::BOOL));
                    }else{
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::VAR));
                    }
                    currToken.clear();
                    startsVar = false;
                }
                else if(startsNum){
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::CONST));
                    currToken.clear();
                    startsNum = false;
                }
            }
                tokens.push_back(Token(column, line, std::string(1, ch), TokenType::RBRACE));
                break;

            case '!':
                if (!currToken.empty()) {
                if(startsVar){
                    if((currToken == "while")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::WHILE));
                    }else if((currToken == "if")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::IF));
                    }else if((currToken == "else")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::ELSE));
                    }else if((currToken == "print")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::PRINT));
                    }else if((currToken == "true") || (currToken == "false")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::BOOL));
                    }else{
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::VAR));
                    }
                    currToken.clear();
                    startsVar = false;
                }
                else if(startsNum){
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::CONST));
                    currToken.clear();
                    startsNum = false;
                }
            }
                if(i != input.size()-1){
                    if(input[i+1] == '='){
                        tokens.push_back(Token(column, line, std::string(1, ch) + std::string(1, input[i+1]), TokenType::EQUAL));
                        i++;
                        column++;
                        break;
                    }else{
                        std::cout << "Syntax error on line " << line << " column " << column << "." << std::endl;
                        currToken.clear();
                    if(err){
                        exit(1);
		            }else{
			            return std::vector<Token>{Token{0,0,"",TokenType::ERR}};
		            }

                    }
                }

                std::cout << "Syntax error on line " << line << " column " << column << "." << std::endl;
                currToken.clear();
                if(err){
                    exit(1);
		        }else{
			        return std::vector<Token>{Token{0,0,"",TokenType::ERR}};
		        }
                break;


            case '=':
                // If there's an accumulated number, create a token for it.
                if (!currToken.empty()) {
                if(startsVar){
                    if((currToken == "while")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::WHILE));
                    }else if((currToken == "if")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::IF));
                    }else if((currToken == "else")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::ELSE));
                    }else if((currToken == "print")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::PRINT));
                    }else if((currToken == "true") || (currToken == "false")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::BOOL));
                    }else{
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::VAR));
                    }
                    currToken.clear();
                    startsVar = false;
                }
                else if(startsNum){
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::CONST));
                    currToken.clear();
                    startsNum = false;
                }
            }
                if(i != input.size() - 1){
                    if(input[i+1] == '='){
                        tokens.push_back(Token(column, line, std::string(1, ch) + std::string(1,input[i+1]), TokenType::EQUAL));
                        i++;
                        column++;
                        break;
                    }
                }
                
                tokens.push_back(Token(column, line, std::string(1, ch), TokenType::ASSIGN));
                
                break;
            // For decimal points.
            case '.':
                // Check for multiple decimal points in the current number.
                isPer = true;
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
                    if((currToken == "while")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::WHILE));
                    }else if((currToken == "if")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::IF));
                    }else if((currToken == "else")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::ELSE));
                    }else if((currToken == "print")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::PRINT));
                    }else if((currToken == "true") || (currToken == "false")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::BOOL));
                    }else{
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::VAR));
                    }
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
                    if((currToken == "while")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::WHILE));
                    }else if((currToken == "if")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::IF));
                    }else if((currToken == "else")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::ELSE));
                    }else if((currToken == "print")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::PRINT));
                    }else if((currToken == "true") || (currToken == "false")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::BOOL));
                    }else{
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::VAR));
                    }
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
                    isPer = false;
                    if(!(startsNum || startsVar)){
                        startsNum = true;
                    }
                    currToken += ch;
                } 
                // If it's an invalid character, create an error token.
                else if(isalpha(ch) || ch == '_'){
                    if(!(startsNum || startsVar)){
                        startsVar = true;
                        isPer = false;
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
                    if((currToken == "while")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::WHILE));
                    }else if((currToken == "if")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::IF));
                    }else if((currToken == "else")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::ELSE));
                    }else if((currToken == "print")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::PRINT));
                    }else if((currToken == "true") || (currToken == "false")){
                        tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::BOOL));
                    }else{
                    tokens.push_back(Token(column - currToken.size(), line, currToken, TokenType::VAR));
                    }
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
