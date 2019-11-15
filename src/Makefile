all: clean compiler

compiler:
	g++ main.cpp parser/Lexer.cpp parser/Parser.cpp parser/TreeNode.cpp parser/Token.cpp semantic/Scope.cpp semantic/Semantic.cpp -o compiler -std=c++17

clean: 
	rm -rf compiler *.o
