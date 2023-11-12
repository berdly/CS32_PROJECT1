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

grove.o: tree.o src/lib/grove.cpp
	${CXX} $(CXXFLAGS)  $^ -o $@

tree.o: lex.o src/lib/parser.cpp
	${CXX} $(CXXFLAGS)  $^ -o $@

lex.o: src/lib/lexer.cpp
	${CXX} $(CXXFLAGS)  $^ -c -o $@