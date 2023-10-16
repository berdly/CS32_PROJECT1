// Main function for testing.
#include "lib/lexer.h"
#include <cstdlib>
#include <cctype>
#include <sstream>

int main() {
    char* raw{new char[1000]};
    std::cin.read(raw, 1000);
    std::string input{raw};
    delete raw;
    
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
