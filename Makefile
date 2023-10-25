CXXFLAGS = -g -std=c++17 -Wall -Wextra -Werror
CXX      = g++


parser: parse
	./parse

lexer: lex
	./lex

calc: cal
	./cal

parse: src/lib/error.h src/lib/lexer.cpp src/lib/lexer.h src/lib/parser.cpp src/lib/parser.h src/lib/token.h src/parse.cpp src/lib/grove.h src/lib/grove.cpp
	${CXX} $(CXXFLAGS)  $^ -o $@


cal: src/lib/error.h src/lib/lexer.cpp src/lib/lexer.h src/lib/parser.cpp src/lib/parser.h src/lib/token.h src/calc.cpp src/lib/grove.h src/lib/grove.cpp
	${CXX} $(CXXFLAGS)  $^ -o $@


lex: src/lib/error.h src/lib/lexer.cpp src/lib/lexer.h src/lib/token.h src/lex.cpp
	${CXX} $(CXXFLAGS)  $^ -o $@
