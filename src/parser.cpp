#include "parser.h"


ASTree::ASTree(const std::vector<Token>& tokens) {
    /*
    for(size_t i = 0 ; i < tokens.size();i++){
        Token check = tokens.at(i);

        if(i == 0){

            if(check.get_type() == TokenType::CONST and tokens.size()!= 1){
                throw ParseError(check);
            }

            proot.pdata = check;

            continue;
        }

        if(check.get_type() == TokenType::PAR){

            proot.buildHelp(tokens, i);

        }

    }*/
    //ERROR CHECKING GOES HERE
    this->proot = this->build(tokens, 0, tokens.size() - 1);
}

ASTree::ASNode build(const std::vector<Token>& tokens, start, end){
    ASTree::ASNode curr{tokens[start]};
    
    switch(curr.get_type()){
        case TokenType::PAR:
            
            break;
        case TokenType::CONST:
            return curr;
        default:
            throw ParserError(tokens[start]);
    }
    
}

void ASNode::buildHelp(const std::vector<Token>& tokens, int idx){

    if(tokens.at(idx).get_type() == TokenType::CONST){
        ASTree::ASNode add = ASTree::ASNode(tokens.at(idx))
        self.pchildren.push_back(add);
        

    }


}

ASTree::ASNode::ASNode(Token data) : pdata{data}, pchildren{} {}
