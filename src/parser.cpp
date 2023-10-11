#include "parser.h"


ASTree::ASTree(const std::vector<Token>& tokens){
    /*
    for(size_t i = 0 ; i < tokens.size();i++){
        Token check = tokens.at(i);

        if(i == 0){

            if(check.get_type() == TokenType::CONST and tokens.size()!= 1){
                throw ParseError("Parse error");
            }

            proot.pdata = check;

            continue;
        }

        if(check.get_type() == TokenType::PAR){

            proot.buildHelp(tokens, i);

        }

    }*/
    Token

}


void ASnode::buildHelp(const std::vector<Token>& tokens, int idx){

    if(tokens.at(idx).get_type() == TokenType::CONST){
        ASTree::ASnode add = ASTree::ASnode(tokens.at(idx))
        self.pchildren.push_back(add);
        

    }


}

ASTree::ASnode::ASnode(Token data) : pdata{data}, pchildren{} {}
