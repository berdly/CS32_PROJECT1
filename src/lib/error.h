#ifndef ERRORS_H
#include <string>
#include "token.h"

class ParserError : public std::exception {
    public:
        Token etoken;
        ParserError(Token token) : etoken{token} {}
      char* what () {
        std::string msg{"Unexpected token at line "};
        msg.append(std::to_string(this->etoken.get_line());
        msg.append(" column ");
        msg.append(std::to_string(this->etoken.get_col());
        msg.push_back(': ');
        msg.append(this->etoken.get_text());
        return const_cast<char*>(msg.c_str());
        ;
    }
};
class LexerError : public std::exception {
    public:
        Token etoken;
        LexerError(Token token) : etoken{token} {}
      char* what () {
        std::string msg{"Syntax error on line "};
        msg.append(std::to_string(this->etoken.get_line());
        msg.append(" and column ");
        msg.append(std::to_string(this->etoken.get_col());
        msg.push_back('.');
        return const_cast<char*>(msg.c_str());
        ;
    }
};
#endif
