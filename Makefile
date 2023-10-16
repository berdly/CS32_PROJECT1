CXXFLAGS = -g -std=c++17 -Wall -Wextra -Werror
CXX      = g++


test: run
	./run

run: src/lib/error.h src/lib/lexer.cpp src/lib/lexer.h src/lib/parser.cpp src/lib/parser.h src/lib/token.h src/parse.cpp
	${CXX} $(CXXFLAGS)  $^ -o $@
