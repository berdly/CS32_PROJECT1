# CS32_PROJECT1
Authors: Jakob Pinedo, Sawyer Rice, David Chang, and Jaren Lowe

These programs should be capable of formatting simple s expressions in standard input into an abstract syntax tree for further calculations or other manipulations of arbitrary nature. It considers each symbol or number as a token which is represented by a designated Token class. This class contains the column, row, text of the token, and includes information on the semantic meaning of the token. These types are enumerated through an enum class TokenTypes which has further explanation in the respective header file, Token.h.

The Lexer which parses the input into a series of tokens is implemented as a single function “reader” which is declared in lexer.h and implemented in src/lib/lexer.cpp. This function moves through the input and identifies valid and invalid tokens, throwing an error in the event of invalid ones. 

The Parser organizes the tokens into a tree. This is represented by the class ASTree which contains the nested class ASNode. The tree possesses a root ASnode, and each node possess a Token and a std::vector<Token> containing its children. These are build recursively from the output of the reader function. Further details can be found in the implementation file src/lib/parser.cpp.

Custom errors have also been implemented for usage within the programs which can be found at src/lib/error.h. Further details on these errors can be found there. 

The programs src/lex.cpp and src/parse.cpp act as tests of the lexing and parsing logic, taking in input from std::cin and outputting it in clear ways which demonstrate the functions of these two sides of the system.

In order to run src/lex.cpp, the implementation of the lexer in src/lib/lexer.cpp must also be compiled and the parser in src/parse.cpp relies on src/lib/parser.cpp as well as the lexer implementation.

Make files for the parse and lexer have been implemented and can be accessed with the commands “make parser” and “make lexer”.
