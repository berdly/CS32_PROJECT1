#include "parser.h"


ASTree::ASTree(const std::vector<Token>& tokens) {
    this->proot = this->build(tokens, 0, tokens.size() - 1);
}

std::vector<std::pair<int>> ASTree::get_child_idx(const std::vector<Token>& tokens, start, end){
    /*TODO
    should take a list of tokens assumed to be inside the parentheses of an operator and produce a list of start and end points based on locations of left and right parentheses
    this is used to determine the number of and location of child nodes in the list.
    for example given 2 (* 4 3 2) 3 
    would output [(0.0), (1,6), (7,7)]
    */
}

ASTree::ASNode build(const std::vector<Token>& tokens, start, end){
    ASTree::ASNode curr{tokens[start]};
    
    switch(curr.get_type()){
        case TokenType::LPAR:
            if(tokens[start+1] != TokenType::EXP){
                throw ParserError(tokens[start+1]);
            }
            else if(tokens[end] != TokenType::RPAR){
                throw ParserError(tokens[end]);
            }
            std::vector<std::pair<int>> child_idx_list{this->get_child_idx(tokens, start+2, end-1)};
            for(const std::pair<int>& child_idx : child_idx_list){
                curr.add_child(build(child_idx(tokens, child_idx.first, child_idx.second)));
                }
            break;
        case TokenType::CONST:
            return curr;
        default:
            throw ParserError(tokens[start]);
    }   
}

ASTree::ASNode::ASNode(Token data) : pdata{data}, pchildren{} {}

void ASTree::ASNode::add_child(ASNode child) { this->pchildren.push_back(child); }
