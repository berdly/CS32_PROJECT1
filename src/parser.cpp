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
    int pdepth{};
    std::vector<std::pair<int>> child_idx{};
    int currstart{start};
    int currend{end};
    for(const Token& token : tokens){
        switch(token.get_type()){
            case TokenType::CONST:
                child_idx.push_back(std::pair<int>(currstart,currstart));
                currstart++;
                break;
            case TokenType::LPAR:
                pdepth++;
                break;
            case TokenType::RPAR:
                pdepth--;
                if(pdepth == 0){
                    child_idx.push_back(std::pair<int>(currstart,currend));
                    currstart = currend + 1;
                }
                else if(pdepth < 0){
                    throw ParserError(token);
                }
                break;
            case TokenType::EXP:
                break;
            case TokenType::ERR:
                throw ParserError(token);
                break;
            currend++;
        }
                
            
                
    }
}

ASTree::ASNode build(const std::vector<Token>& tokens, start, end){
    ASTree::ASNode curr{tokens[start]};
    
    switch(curr.get_type()){
        case TokenType::LPAR:
            //we know it should be an operand
            if(tokens[start+1] != TokenType::EXP){
                throw ParserError(tokens[start+1]);
            }
            else if(tokens[end] != TokenType::RPAR){
                throw ParserError(tokens[end]);
            }
            //create node for operand and ignore parentheses
            ASTree::ASNode rootNode{tokens[start+1]};
            //find where children begin and end
            std::vector<std::pair<int>> child_idx_list{this->get_child_idx(tokens, start+2, end-1)};
            //recursively add children while properly building out their children
            for(const std::pair<int>& child_idx : child_idx_list){
                rootNode.add_child(build(tokens, child_idx.first, child_idx.second));
                }
            return rootNode;
        case TokenType::CONST:
            //const has no children so can simply return
            return curr;
        default:
            //should not start with anything but CONST or LPAR
            throw ParserError(tokens[start]);
    }   
}

ASTree::ASNode::ASNode(Token data) : pdata{data}, pchildren{} {}

void ASTree::ASNode::add_child(ASNode child) { this->pchildren.push_back(child); }
