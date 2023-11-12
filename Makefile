CXXFLAGS = -g -std=c++17 -Wall -Wextra -Werror
CXX      = g++

scrypt: grove.o scrypt.cpp
	${CXX} $(CXXFLAGS)  $^ -o $@

format: grove.o format.cpp
	${CXX} $(CXXFLAGS)  $^ -o $@
calc: grove.o calc.cpp
	${CXX} $(CXXFLAGS)  $^ -o $@

lex: lex.o lex.cpp
	${CXX} $(CXXFLAGS)  $^ -o $@

grove.o: tree.o src/lib/grove.h src/lib/grove.cpp
	${CXX} $(CXXFLAGS)  $^ -c $@

tree.o: lex.o src/lib/parser.cpp src/lib/parser.h
	${CXX} $(CXXFLAGS)  $^ -c $@

lex.o: src/lib/error.h src/lib/lexer.cpp src/lib/lexer.h src/lib/token.h
	${CXX} $(CXXFLAGS)  $^ -c $@