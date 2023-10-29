#include <iostream>
#include <iomanip>
#include <string>
#include "../src/lib/lexer.h"
#include "../src/lib/parser.h"
#include "../src/lib/error.h"
#include "../src/lib/grove.h"

 int main(){
    std::string input;
    std::string one_back;
    ASGrove grove{};
    while(true){
       getline(std::cin, input);
        
        if (!input.empty()) {
            input +="\n"; // Concatenating each line with a newline character
        }
     
        auto token_list{reader(input, false)};
        if(!input.empty()){
          one_back = input;
        }
        input.clear(); // Clearing previous inputs for a new tree to be created
        
        try{
            if((token_list.size() == 1) && token_list.at(0).get_type() == TokenType::ERR){}
            else if(!token_list.empty()){
            grove.add_tree(ASTree{token_list, true});
            grove.print();
            grove.calc();
            }
            else{
                return 0;
            }
        
        }catch(const ParserError& e){
            //handles parser errors and displays error messages, else block is for early endings since the END token is not on the actual vector and must be imagined next to the final token
            if(e.etype == PErrType::NORM){         
            std::cout << e.new_what() << '\n';
            }
            else{
                unsigned extra{};
                if(one_back.at(one_back.size() - 2) == ' '){
                  extra++;
                }
                std::cout << "Unexpected token at line " << e.etoken.get_line() << " column " << e.etoken.get_col() + e.etoken.get_text().size() + extra << ": END\n";
            }
        }
        catch(const ZeroDivision&){
            std::cout << "Runtime error: division by zero." << '\n';
        }
        catch(const IdentifierError& e){
            std::cout<<e.new_what()<<'\n';
        }
        catch(const std::out_of_range& e){
            std::cout << e.what() << '\n';
        }
    }
 }

