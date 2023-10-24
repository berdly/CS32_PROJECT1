CXXFLAGS = -g -std=c++17 -Wall -Wextra -Werror
CXX      = g++


parser: parse
	./parse

lexer: lex
	./lex

parse: src/lib/error.h src/lib/lexer.cpp src/lib/lexer.h src/lib/parser.cpp src/lib/parser.h src/lib/token.h src/test.cpp src/lib/grove.h src/lib/grove.cpp
	${CXX} $(CXXFLAGS)  $^ -o $@


lex: src/lib/error.h src/lib/lexer.cpp src/lib/lexer.h src/lib/token.h src/lex.cpp
	${CXX} $(CXXFLAGS)  $^ -o $@
