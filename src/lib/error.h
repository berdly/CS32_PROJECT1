#ifndef ERROR_H
#define ERROR_H
#include <string>
#include "token.h"
//used by program to automatically display an error, gets passed the offending token and produces a string with the error message with the new_what method
//same for both types
//the PErrType enum class designates whether an error occured in the token vector itself in NORM or though the absence of a token in END
enum class PErrType{
    NORM,
    END,
};
class ParserError : public std::exception {
    public:
        Token etoken;
        PErrType etype;
        ParserError(Token token) : etoken{token}, etype{PErrType::NORM} {}
        ParserError(Token token, PErrType type) : etoken{token}, etype{type} {}
      std::string new_what () const{
        std::string msg{"Unexpected token at line "};
        msg.append(std::to_string(this->etoken.get_line()));
        msg.append(" column ");
        msg.append(std::to_string(this->etoken.get_col()));
        msg.append(": ");
        msg.append(this->etoken.get_text());
        return msg;
    }
};
class LexerError : public std::exception {
    public:
        Token etoken;
        LexerError(Token token) : etoken{token} {}
      std::string new_what () const{
        std::string msg{"Syntax error on line "};
        msg.append(std::to_string(this->etoken.get_line()));
        msg.append(" and column ");
        msg.append(std::to_string(this->etoken.get_col()));
        msg.push_back('.');
        return msg;
        ;
    }
};
//very simple solution for catching this case
class ZeroDivision : public std::exception {};

class IdentifierError: public std::exception{
        public:
        Token etoken;
        IdentifierError(Token token) : etoken{token} {}
      std::string new_what () const{
        std::string msg{"Runtime error: unknown identifier "};
        msg.append(this->etoken.get_text());
        return msg;
        
    }
};

class TypeError: public std::exception{
        public:
        Token etoken;
        TypeError(Token token) : etoken{token} {}
      std::string new_what () const{
        std::string msg{"Runtime error: invalid operand type."};
        msg.append(this->etoken.get_text());
        return msg;
        
    }
};
class NotImplemented: public std::exception {}
#endif
