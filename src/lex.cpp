#include "lexer.cpp"

int main() {
    std::string input;
    int fline;

    // Prompt the user for input.
    std::cout << "Enter the expression: ";
    std::getline(std::cin, input);

    // Parse the input and get the tokens.
    std::vector<Token> tokens = reader(input);  // Change to vector
    
    // Display the tokens.
    for (const Token& t : tokens) {
        std::cout << t.get_line() << "  " << t.get_col() << "  " << t.get_text() << std::endl;
        fline = t.get_line();
    }
    std::cout<<fline+1<<"   1  END";
    return 0;
}
