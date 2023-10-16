#include <iostream>
#include <string>
#include "lib/lexer.h"
#include "lib/parser.h"
#include "lib/error.h"


int main(){
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
    
    try{
    ASTree tree{reader(input)};
    tree.print();
    std::cout << '\n';
    std::cout << tree.calc();
    }
    catch(const ParserError& e){
        std::cout << e.new_what();
        return 2;
    }
    catch(const ZeroDivision&){
        std::cout << "Runtime error: division by zero.";
        return 3;
    }
}
