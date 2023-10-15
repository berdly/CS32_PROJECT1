#include <iostream>
#include <string>
#include "lexer.h"
#include "parser.h"
#include "errors.h"


int main(){
    std::string input{};
    std::cin >> input;
    try{
    ASTree tree{reader(input)};
    tree.print();
    std::cout << '\n';
    std::cout << tree.calc();
    }
    catch(const ParserError& e){
        std::cout << e.what();
    }
    catch(const ZeroDivision&){
        std::cout << "Runtime error: division by zero.";
    }
}
