#include <iostream>
#include <iomanip>
#include <string>
#include "../src/lib/lexer.h"
#include "../src/lib/parser.h"
#include "../src/lib/error.h"
#include "../src/lib/grove.h"

 int main(){
    std::vector<ASTree> statements{};
    std::string input, line;
    //bool l = false;  // Initialize l to false'
    int linecount{};
    

     while (true) {
        input.clear();
        //linecount = 0;
        bool flag = true;
        
        while(flag){

        getline(std::cin, line);
        
        if (!line.empty()) {
            input += line + "\n"; // Concatenating each line with a newline character
            //l = true;
        }
        if (std::cin.eof()) {
            
            break;
        }
        linecount++;
        getline(std::cin, line);

        if(line == "\n"){
            flag = false;
           
        }
        }
        //l = false;
        //std::cout<<input<<std::endl;

        auto tokens{reader(input)};

        if(linecount > 1){
         for(auto& token: tokens){
          token.change_line(linecount);
         }
        }
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
                //std::cout<<"THORW 3";
            std::cout << e.new_what() << '\n';
            }
            else{
                //std::cout << "Unexpected token at line " << e.etoken.get_line() << " column " << e.etoken.get_col() + e.etoken.get_text().size() << ": END\n";
              std::cout << e.new_what() << '\n';
            }
            return 2;
        }
        catch(const ZeroDivision&){
            std::cout << "Runtime error: division by zero." << '\n';
            return 3;
        }
        catch(const IdentifierError& e){
            std::cout<<e.new_what()<<"\n";
            return 3;
        }
    }

     ASGrove grove{statements};
    for(size_t i = 0; i < statements.size();i++){
    grove.print();
    grove.eval(); 
    } 
  
    
        
    }



   
        
    //read in token vector 
    
    
    

