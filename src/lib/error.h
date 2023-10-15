#ifndef ERRORS_H
#include <string>
#include "token.h"

class ParserError : public std::exception {
    public:
        Token etoken;
        ParserError(Token token) : etoken{token} {}
      std::string new_what () const{
        std::string msg{"Unexpected token at line "};
        msg.append(std::to_string(this->etoken.get_line());
        msg.append(" column ");
        msg.append(std::to_string(this->etoken.get_col());
        msg.append(": ");
        msg.append(this->etoken.get_text());
        return msg;
        ;
    }
};
class LexerError : public std::exception {
    public:
        Token etoken;
        LexerError(Token token) : etoken{token} {}
      char* new_what () const{
        std::string msg{"Syntax error on line "};
        msg.append(std::to_string(this->etoken.get_line());
        msg.append(" and column ");
        msg.append(std::to_string(this->etoken.get_col());
        msg.push_back('.');
        return msg;
        ;
    }
};

class ZeroDivision : public std::exception {
};
#endif
