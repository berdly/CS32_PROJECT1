#include <iostream>
#include <iomanip>
#include <string>
#include "../src/lib/lexer.h"
#include "../src/lib/parser.h"
#include "../src/lib/error.h"
#include "../src/lib/grove.h"

int main(){
    std::string input, line;
    //bool l = false;  // Initialize l to false'
    int linecount{};
    std::vector<ASTree> statements{};

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
        auto tokens{reader(input)};


        try{
            //if there are tokens, build tree
            if(!tokens.empty()){
                ASTree tree{tokens};
                statements.push_back(tree);
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

    ASGrove grove{statements};
    grove.print();
    std:: cout<< grove.eval();
 
 
    /* auto tokens1{reader(" ( = a ( + 5 6 )) ")};
    auto tokens2{reader(" ( * a 3) ")};
    ASTree tree1{tokens1};
    ASTree tree2{tokens2};
    //auto tokens2{reader(input)};
    //auto tokens3{reader(input)};
    statements.push_back(tree1);
    statements.push_back(tree2);
    //statements.push_back(tokens3);

    ASGrove grove{statements};
    std::cout<<grove.eval()<<std::endl;
    //read in token vector */
    
    
    
}
