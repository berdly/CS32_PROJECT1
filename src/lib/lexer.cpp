#include "lexer.h"
#include <cstdlib>
#include <cctype>
#include <sstream>

// Function to parse the input string and create tokens for each number and operand.
std::vector<Token> reader(const std::string& input) {  // Change return type to vector
    std::vector<Token> tokens;  // Use vector instead of stack to store generated tokens
    
    // Temporary string to accumulate digits of a number.
    std::string currentNumber;
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
                if (!currentNumber.empty()) {
                    tokens.push_back(Token(column - currentNumber.size(), line, currentNumber, TokenType::CONST));
                    currentNumber.clear();
                }
                // Create a token for the operator.
                tokens.push_back(Token(column, line, std::string(1, ch), TokenType::EXP));
                break;

            // For left parenthesis.
            case '(':
                // If there's an accumulated number, create a token for it.
                if (!currentNumber.empty()) {
                    tokens.push_back(Token(column - currentNumber.size(), line, currentNumber, TokenType::CONST));
                    currentNumber.clear();
                }
                // Create a token for the left parenthesis.
                tokens.push_back(Token(column, line, std::string(1, ch), TokenType::LPAR));
                break;

            // For right parenthesis.
            case ')':
                // If there's an accumulated number, create a token for it.
                if (!currentNumber.empty()) {
                    tokens.push_back(Token(column - currentNumber.size(), line, currentNumber, TokenType::CONST));
                    currentNumber.clear();
                }
                // Create a token for the right parenthesis.
                tokens.push_back(Token(column, line, std::string(1, ch), TokenType::RPAR));
                break;

            // For decimal points.
            case '.':
                // Check for multiple decimal points in the current number.
                if (currentNumber.find('.') != std::string::npos) {
                   std::cout << "Syntax error on line " << line << " column " << column << "." << std::endl;
                   currentNumber.clear();
                   exit(1);
                }
                // Check for trailing decimals.
                else if (i + 1 == input.size() || isspace(input[i + 1]) || input[i + 1] == '\n') {
                    std::cout << "Syntax error on line " << line << " column " << column+1 << "." << std::endl;
                    currentNumber.clear();
                    exit(1);
               }
                // Check for leading decimals.
               else if (currentNumber.empty()) {
                   std::cout << "Syntax error on line " << line << " column " << column << "." << std::endl;
                   currentNumber.clear();
                   exit(1);
               } else {
                   currentNumber += ch;
                }
               break;
            
            // For newline characters.
            case '\n':
            // If there's an accumulated number, create a token for it.
            if (!currentNumber.empty()) {
                tokens.push_back(Token(column - currentNumber.size(), line, currentNumber, TokenType::CONST));
                currentNumber.clear();
            }
                line++;
                column = 0;
                break;

            case ' ':
                // White space seperate
                if (!currentNumber.empty()) {
                    tokens.push_back(Token(column - currentNumber.size(), line, currentNumber, TokenType::CONST));
                    currentNumber.clear();
                }
                break;               
            
            // For other characters.
            default:
                // If it's a digit, add to the current number.
                if (isdigit(ch)) {
                    currentNumber += ch;
                } 
                // If it's an invalid character, create an error token.
                else if (isalpha(ch) || !isspace(ch)) {
                    std::cout<<"Syntax error on line "<< line <<" column " << column << "." << std::endl;
                    currentNumber.clear();
                    exit(1);
                }
                break;
        }
        column++;
    }

    // If there's any remaining number, create a token for it.
    if (!currentNumber.empty()) {
        tokens.push_back(Token(column - currentNumber.size(), line, currentNumber, TokenType::CONST));
    }

    return tokens;
}
