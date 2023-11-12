CXXFLAGS = -g -std=c++17 -Wall -Wextra -Werror
CXX      = g++

scrypt: lex.o tree.o grove.o scryp.o
	${CXX} $(CXXFLAGS)  $^ -o $@

format: grove.o src/lib/format.cpp
	${CXX} $(CXXFLAGS)  $^ -o $@
calc: grove.o src/lib/calc.cpp
	${CXX} $(CXXFLAGS)  $^ -o $@

lex: lex.o
	${CXX} $(CXXFLAGS)  $^ -o $@

scrypt.o: src/lib/scrypt.cpp
	${CXX} $(CXXFLAGS)  $^ -c -o $@
grove.o: src/lib/grove.cpp
	${CXX} $(CXXFLAGS)  $^ -c -o $@

tree.o: src/lib/parser.cpp
	${CXX} $(CXXFLAGS)  $^ -c -o $@

lex.o: src/lib/lexer.cpp
	${CXX} $(CXXFLAGS)  $^ -c -o $@