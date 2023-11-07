# CS32_PROJECT1
Authors: Jakob Pinedo, Sawyer Rice, David Chang, and Jaren Lowe

This project seeks to implement a simple dynamically typed programming language using abstract syntax trees.

The process begins with lexing the input. The input's characters are parsed into groups based on their semantic meanings and stored in instances of class Token. Each token contains the text of the characters, their semantic meaning represented by a member of enum class TokenType, and place in the text for error messages. 

Parsing on the expression level is handled by the class ASTree. An array of tokens is pased to the constructor and parsed into a single properly grouped expression based on precedence rules. This class has a member which is either empty or the root node of a valid expression. 

Parsing on the block level is handled by the heart of the library, ASGrove. It is passed an array of arrays of Tokens which represent individual statements. If a statement is simply an expression, it's passed off to the ASTree constructor and stored as an ASTree, and if the statement is a more complex one like and if or while, then it's split into two parts, the conditional which is passed to the ASTree constructor and the body which is handled as an entirely new grove by recursively calling the ASGrove constructor. 

These two parts are then stored in a StatementTree object which inherits from ASTree. The StatementTree also has a next member which points to another StatementTree allowing linked list functionality which is used for chaining ifs, elses, and elseifs. The various ASTrees and StatementTrees generated from parsing the input are then stored in order in a vector of ASTree* which can then be evaluated. The types of the respective trees are also held using a vector of the enum TreeTypes which helps with evaluating the trees correctly.

Data for variables is stored as std::variant<double, bool> for simplicity and type safety. This is typedef'ed as Var for simplicity.

Evaluating the trees is handled in the ASGrove::eval() function. Any variables that are created are saved in the std::map<std::string, Var> vars which allows for accessing the data by name.

The ASTree can also be printed using the situational print functions defined in the class.

Custom errors have also been implemented for usage within the programs which can be found at src/lib/error.h. Further details on these errors can be found there. 

The program src/lex.cpp acts as a test of the lexing and parsing logic, taking in input from std::cin and outputting it in clear ways which demonstrate correct functionality.

The program src/calc.cpp acts as a simple interpreter for basic math problems and supports variable assignment.

The program src/scrypt.cpp allows for execution of arbitrary code within the parameters of the language.

Make files for all these programs have been implemented and can be accessed with the command "make *insert_program_name*"
