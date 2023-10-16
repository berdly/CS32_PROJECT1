#include "../src/lib/lexer.h"
#include <iomanip> 

int main() {
    std::string input, line;
    bool l = false;  // Initialize l to false

    while (true) {
        getline(std::cin, line);
        if (!line.empty()) {
            input += line + "\n"; // Concatenating each line with a newline character
            l = true;
        }
        if (std::cin.eof()) {
            break;
        }
        l = false;
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
    if (!tokens.empty() && l == false) {  // Use == for comparison
        std::cout << std::right << std::setw(4) << tokens.back().get_line() + 1 
                  << "    1  END" << std::endl;
    } else if (!tokens.empty() && l == true) {  // Use == for comparison and correct the else if syntax
        std::cout << std::right << std::setw(4) << tokens.back().get_line() 
                  << "    1  END" << std::endl;
    } else {
        std::cout << "   1    1  END" << std::endl; // If there are no tokens, default to line 1 
    }

    return 0;
}
