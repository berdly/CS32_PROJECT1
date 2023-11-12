CXXFLAGS = -g -std=c++17 -Wall -Wextra -Werror
CXX      = g++

scrypt: lex.o tree.o grove.o scrypt.o
	${CXX} $(CXXFLAGS)  $^ -o $@

format: lex.o tree.o grove.o format.o
	${CXX} $(CXXFLAGS)  $^ -o $@
calc: lex.o tree.o grove.o calc.o
	${CXX} $(CXXFLAGS)  $^ -o $@

lex: lex.o
	${CXX} $(CXXFLAGS)  $^ -o $@

calc.o: src/calc.cpp
	${CXX} $(CXXFLAGS)  $^ -c -o $@
format.o: src/format.cpp
	${CXX} $(CXXFLAGS)  $^ -c -o $@
scrypt.o: src/scrypt.cpp
	${CXX} $(CXXFLAGS)  $^ -c -o $@

grove.o: src/lib/grove.cpp
	${CXX} $(CXXFLAGS)  $^ -c -o $@

tree.o: src/lib/parser.cpp
	${CXX} $(CXXFLAGS)  $^ -c -o $@

lex.o: src/lib/lexer.cpp
	${CXX} $(CXXFLAGS)  $^ -c -o $@

clean:
	rm -f *.o lex format calc