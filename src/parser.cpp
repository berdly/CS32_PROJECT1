#include "parser.h"


ASTree::ASTree(const std::vector<Token>& tokens){

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

    }

}


void ASnode::buildHelp(const std::vector<Token>& tokens, int idx){

    if(tokens.at(idx).get_type() == TokenType::CONST){
        ASnode add = ASnode(tokens.at(idx))
        self.pchildren.push_back(add);
        

    }


}

ASnode::ASnode(Token pdata){
    self.pdata = pdata;
    pchildren = std::vector<ASnode>();
}
