CXXFLAGS = -g -std=c++17 -Wall -Wextra -Werror
CXX      = g++


format: forma
	./forma

lexer: lex
	./lex

calc: cal
	./cal

forma: src/lib/error.h src/lib/lexer.cpp src/lib/lexer.h src/lib/parser.cpp src/lib/parser.h src/lib/token.h src/format.cpp src/lib/grove.h src/lib/grove.cpp
	${CXX}   $^ -o $@


cal: src/lib/error.h src/lib/lexer.cpp src/lib/lexer.h src/lib/parser.cpp src/lib/parser.h src/lib/token.h src/calc.cpp src/lib/grove.h src/lib/grove.cpp
	${CXX} $(CXXFLAGS)  $^ -o $@


lex: src/lib/error.h src/lib/lexer.cpp src/lib/lexer.h src/lib/token.h src/lex.cpp
	${CXX} $(CXXFLAGS)  $^ -o $@
