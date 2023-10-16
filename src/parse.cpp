#include <iostream>
#include <string>
#include "lib/lexer.h"
#include "lib/parser.h"
#include "lib/error.h"


int main(){
    std::string input{};
    std::string fullstr{};
    std::getline(std::cin, input);
    while(input.size() > 0){
        fullstr.append(input);
        input.clear();
        std::getline(std::cin, input);
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
