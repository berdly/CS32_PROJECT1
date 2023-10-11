#ifndef ERRORS_H
#include <string>

class ParserError : public std::exception {
    public:
      char* what (const Token& token) {
        std::string msg{"Unexpected token at line "};
        msg.append(std::to_string(token.get_line());
        msg.append(" column ");
        msg.append(std::to_string(token.get_col());
        msg.push_back(': ');
        msg.append(token.get_text());
        return msg.c_string();
        ;
    }
};
class LexerError : public std::exception {
    public:
      char* what (const Token& token) {
        std::string msg{"Syntax error on line "};
        msg.append(std::to_string(token.get_line());
        msg.append(" and column ");
        msg.append(std::to_string(token.get_col());
        msg.push_back('.');
        return msg.c_string();
        ;
    }
};
