#include <iostream>
#include <iomanip>
#include <string>
#include "../src/lib/lexer.h"
#include "../src/lib/parser.h"
#include "../src/lib/error.h"


int main(){
    std::string input, line;
    //bool l = false;  // Initialize l to false

    while (true) {
        getline(std::cin, line);
        if (!line.empty()) {
            input += line + "\n"; // Concatenating each line with a newline character
            //l = true;
        }
        if (std::cin.eof()) {
            break;
        }
        //l = false;
    }
    
    auto tokens{reader(input)};
    try{
    if(!tokens.empty()){
    ASTree tree{tokens};
    tree.print();
    std::cout << '\n';
    std::cout << tree.calc();
    std::cout << '\n';
    }
    }
    catch(const ParserError& e){
        std::cout << e.new_what() << '\n';
        return 2;
    }
    catch(const ZeroDivision&){
        std::cout << "Runtime error: division by zero.";
        return 3;
    }
}
