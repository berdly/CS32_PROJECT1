#include "lexer.cpp"

int main() {
    std::string input;
    std::vector<Token> tokens;

    // Prompt the user for input.
    std::cout << "Enter the expression: ";
    while (getline(std::cin, input) && !input.empty()) { // Continue taking input until an empty line is entered
        std::vector<Token> new_tokens = reader(input);  // Parse each line and get the tokens
        tokens.insert(tokens.end(), new_tokens.begin(), new_tokens.end()); // Add new tokens to the existing list of tokens
    }

    // Function to display the tokens with right-aligned columns.
    for (const Token& t : tokens) {
        std::cout << std::right << std::setw(4) << t.get_line() << " "
                  << std::right << std::setw(4) << t.get_col() << "  "
                  << t.get_text() << std::endl;
    }

    // Display the END token
    if (!tokens.empty()) { // Ensure there are tokens before trying to access the last token
        std::cout << std::right << std::setw(4) << tokens.back().get_line() + 1 
                  << "    1  END" << std::endl;
    } else {
        std::cout << "   1    1  END" << std::endl; // If there are no tokens, default to line 1
    }

    return 0;
}
