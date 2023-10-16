// Main function for testing.
#include "lib/lexer.h"
#include <cstdlib>
#include <cctype>
#include <sstream>

int main() {
     std::string input{};
    std::string fullstr{};
    std::getline(std::cin, input);
    while(input != "\n"){
        fullstr.append(input);
        input.clear();
        std::getline(std::cin, input);
    }
    
    int fline;

    // Parse the input and get the tokens.
    std::vector<Token> tokens = reader(input);  // Change to vector
    
    // Display the tokens.
    // Use a range-based for loop to iterate over the vector
    for (const Token& t : tokens) {
        std::cout << t.get_line() << "  " << t.get_col() << "  " << t.get_text() << std::endl;
        fline = t.get_line();
    }
    std::cout<<fline+1<<"   1  END";
    return 0;
}
