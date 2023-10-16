#include <iostream>
#include <string>
#include "lib/lexer.h"
#include "lib/parser.h"
#include "lib/error.h"


int main(){
    std::string input{};
    std::string fullstr{};
    while (!std::cin.eof()){
        std::getline(std::cin, input);
        input.clear();
        fullstr.append(input);
    }
    try{
    ASTree tree{reader(fullstr)};
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
