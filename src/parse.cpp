#include <iostream>
#include <iomanip>
#include <string>
#include "../src/lib/lexer.h"
#include "../src/lib/parser.h"
#include "../src/lib/error.h"


int main(){
    std::string input, line;
    //bool l = false;  // Initialize l to false'
    int linecount{};

    while (true) {
        getline(std::cin, line);
        if (!line.empty()) {
            input += line + "\n"; // Concatenating each line with a newline character
            //l = true;
        }
        if (std::cin.eof()) {
            break;
        }
        linecount++;
        //l = false;
    }
    //read in token vector
    auto tokens{reader(input)};
    
    try{
    //if there are tokens, build tree
    if(!tokens.empty()){
    ASTree tree{tokens};
    tree.print();
    std::cout << '\n';
    std::cout << tree.calc();
    std::cout << '\n';
    }
    else{
        std::cout << "Unexpected token at line " << linecount+1 << " column 1: END\n";
        return 2;
    }
    }
    catch(const ParserError& e){
        //handles parser errors and displays error messages, else block is for early endings since the END token is not on the actual vector and must be imagined next to the final token
        if(e.etype == PErrType::NORM){
        std::cout << e.new_what() << '\n';
        }
        else{
        std::cout << "Unexpected token at line " << e.etoken.get_line() << " column " << e.etoken.get_col() + e.etoken.get_text().size() << ": END\n";
        }
        return 2;
    }
    catch(const ZeroDivision&){
        std::cout << "Runtime error: division by zero." << '\n';
        return 3;
    }
}
