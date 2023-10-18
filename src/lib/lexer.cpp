#include "lexer.h"
#include <cstdlib>
#include <cctype>
#include <sstream>

// Function to parse the input string and create tokens for each number and operand.
std::vector<Token> reader(const std::string& input) {  // Change return type to vector
    std::vector<Token> tokens;  // Use vector instead of stack to store generated tokens
    bool startsNum{false};
    bool startsVar{false};
    // Temporary string to accumulate digits of a number.
    std::string currToken;
    int column = 1, line = 1;

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
                if(startsVar){
                   std::cout << "Syntax error on line " << line << " column " << column << "." << std::endl;
                   currToken.clear();
                   exit(1);
                }
                if (currToken.find('.') != std::string::npos) {
                   std::cout << "Syntax error on line " << line << " column " << column << "." << std::endl;
                   currToken.clear();
                   exit(1);
                }
                // Check for trailing decimals.
                else if (i + 1 == input.size() || isspace(input[i + 1]) || input[i + 1] == '\n') {
                    std::cout << "Syntax error on line " << line << " column " << column+1 << "." << std::endl;
                    currToken.clear();
                    exit(1);
               }
                // Check for leading decimals.
               else if (currToken.empty()) {
                   std::cout << "Syntax error on line " << line << " column " << column << "." << std::endl;
                   currToken.clear();
                   exit(1);
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
                        exit(1);
                    }
                    currToken += ch;
                }
                else if (!isspace(ch)) {
                    std::cout<<"Syntax error on line "<< line <<" column " << column << "." << std::endl;
                    currToken.clear();
                    exit(1);
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

/* Main function for testing.
int main() {
    std::string input;
    int fline;

    // Prompt the user for input.
    std::cout << "Enter the expression: ";
    std::getline(std::cin, input);

    // Parse the input and get the tokens.
    std::vector<Token> tokens = reader(input);  // Change to vector
    
    // Display the tokens.
    // Use a range-based for loop to iterate over the vector
    for (const Token& t : tokens) {
        std::cout << t.get_line() << "  " << t.get_col() << "  " << t.get_text() << std::endl;
        fline = t.get_line();
    }
    std::cout<<fline+1<<"   1  END";
    return 1;
}*/
