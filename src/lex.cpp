#include "../src/lib/lexer.h"
#include <iomanip> 
int main() {
    std::string input, line;

    // std::cout << "Enter the expression:" << std::endl;
    while (true) {
        getline(std::cin, line);
        if (line.empty()||std::cin.peek() == EOF) { // Check if line is empty
            break;
        }
        input += line + "\n"; // Concatenating each line with a newline character
    }

    // Parse the input and get the tokens.
    std::vector<Token> tokens = reader(input);  
    
    // Function to display the tokens with right-aligned columns.
    for (const Token& t : tokens) {
        std::cout << std::right << std::setw(4) << t.get_line() << " "
                  << std::right << std::setw(4) << t.get_col() << "  "
                  << t.get_text() << std::endl;
    }

    // Display the END token
    if (!tokens.empty()) { 
        std::cout << std::right << std::setw(4) << tokens.back().get_line() + 1 
                  << "    1  END" << std::endl;
    } else {
        std::cout << "   1    1  END" << std::endl; // If there are no tokens, default to line 1
    }

    return 1;
}
