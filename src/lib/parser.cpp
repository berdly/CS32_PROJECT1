//Thanks to my brother Donald for helping me on this :)
#include "reader.h"
#include <cctype>
#include <sstream>

// Function to parse the input string and create tokens for each number and operand.
std::stack<Token> reader(const std::string& input) {
    // Stack to store the generated tokens.
    std::stack<Token> tokens;
    
    // Temporary string to accumulate digits of a number.
    std::string currentNumber;
    
    // Initialize column and line counters.
    int column = 1, line = 1;

    // Iterate over each character in the input string.
    for (size_t i = 0; i < input.size(); ++i) {
        char ch = input[i];
        
        // Switch-case to handle different characters.
        switch (ch) {
            // For arithmetic operators and parentheses.
            case '+':
            case '-':
            case '*':
            case '/':
            case '(':
            case ')':
                // If there's an accumulated number, create a token for it.
                if (!currentNumber.empty()) {
                    tokens.push(Token(column - currentNumber.size(), line, currentNumber, TokenType::CONST));
                    currentNumber.clear();
                }
                // Create a token for the operator or parenthesis.
                tokens.push(Token(column, line, std::string(1, ch), TokenType::EXP));
                break;
            
            // For decimal points.
            case '.':
                // Check for multiple decimal points in the current number.
                if (currentNumber.find('.') != std::string::npos) {
                    tokens.push(Token(column, line, "Error: multiple decimal points", TokenType::ERR));
                    currentNumber.clear();
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
                    tokens.push(Token(column, line, "Error: invalid character", TokenType::ERR));
                    currentNumber.clear();
                }
                break;
        }
        column++;
    }

    // If there's any remaining number, create a token for it.
    if (!currentNumber.empty()) {
        tokens.push(Token(column - currentNumber.size(), line, currentNumber, TokenType::CONST));
    }

    return tokens;
}

// Main function for testing.
int main() {
    std::string input;
    
    // Prompt the user for input.
    std::cout << "Enter the expression: ";
    std::getline(std::cin, input);

    // Parse the input and get the tokens.
    std::stack<Token> tokens = reader(input);
    
    // Display the tokens.
    while (!tokens.empty()) {
        Token t = tokens.top();
        std::cout << "Token: " << t.get_text() << ", Column: " << t.get_col() << ", Line: " << t.get_line() << std::endl;
        tokens.pop();
    }

    return 0;
}

