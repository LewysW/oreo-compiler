#include <iostream>
#include <map>
#include <regex>
#include "parser/Lexer.h"
#include "parser/Parser.h"
#include "semantic/Semantic.h"
#include "type_checking/TypeChecker.h"

using std::cout;
using std::endl;

int main(int argc, char* argv[]) {
    //If file is provided as argument
    if (argc > 1) {
        //Store filename
        std::string fileName = std::string(argv[1]);

        //Perform lexical analysis on file
        //and print tokens
        Lexer lexer(fileName);
        lexer.printTokens();

        //Perform syntactic analysis on file
        //and print abstract syntax tree
        Parser parser(lexer.getTokenList());
        parser.printTree();

        //Perform semantic analysis on parse tree
        Semantic semantic;
        semantic.analyse(parser.getParseTree());
        //Print scopes and symbol tables
        //semantic.printTree(semantic.getGlobalScope());

        //Perform type checking on parse tree
        TypeChecker typeChecker;
        //typeChecker.checkTypes(parser.getParseTree(), semantic.getGlobalScope());

        //TODO list:
        //TODO - Else not allocated correctly in parser - FIX!

        //TODO - report on type checking

        //TODO - Pass queue through type checking code and add instructions for ease of TAC generation

        //TODO - three address code generation
        //TODO - report on three address code generation
    }

    return 0;
}
