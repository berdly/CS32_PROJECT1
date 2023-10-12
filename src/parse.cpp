#include <iostream>
#include <string>
#include "lexer.h"
#include "parser.h"



int main(){
    std::string input{};
    std::cin >> input;
    ASTree tree{reader(input)};
    tree.print();
    std::cout << '\n';
    try{
    std::cout << tree.calc();
    }
    catch(const ZeroDivision&){
        std::cout << "Runtime error: division by zero.";
    }
}
