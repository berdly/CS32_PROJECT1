#include "lexer.h"
#include <cctype>
#include <sstream>

// Function to parse the input string and create tokens for each number and operand.
std::vector<Token> reader(const std::string& input) {  // Change return type to vector
    std::vector<Token> tokens;  // Use vector instead of stack to store generated tokens
    
    // Temporary string to accumulate digits of a number.
    std::string currentNumber;
    
    // Initialize column and line counters.
    int column = 1, line = 1;

    // Iterate over each character in the input string.
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
                    std::cout<<"Unexpected token at line "<< line <<" column " <<column;
                    currentNumber.clear();
                    return 1;
                } else {
                    currentNumber += ch;
                }
                break;
            
            // For newline characters.
            case '\n':
                line++;
                column = 0;
                break;
            
            // For other characters.
            default:
                // If it's a digit, add to the current number.
                if (isdigit(ch)) {
                    currentNumber += ch;
                } 
                // If it's an invalid character, create an error token.
                else if (isalpha(ch) || !isspace(ch)) {
                    std::cout<<"Unexpected token at line "<< line <<" column " <<column;
                    currentNumber.clear();
                    return 1;
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

// Main function for testing.
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
        std::cout << "Token: " << t.get_text() << ", Column: " << t.get_col() << ", Line: " << t.get_line() << std::endl;
        fLine = t.get_line();
    }
    std::cout<<fLine+1<<"    1  END"
    return 0;
}
