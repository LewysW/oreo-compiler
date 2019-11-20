#include <iostream>
#include <map>
#include <regex>
#include "parser/Lexer.h"
#include "parser/Parser.h"
#include "semantic/Semantic.h"
#include "type_checking/TypeChecker.h"
#include "tac_generation/TAC_Generator.h"

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
        semantic.printTree(semantic.getGlobalScope());

        //Perform type checking on parse tree
        TypeChecker typeChecker;
        typeChecker.checkTypes(parser.getParseTree(), semantic.getGlobalScope());

        //Perform three address code generation
        TAC_Generator tacGenerator;
        tacGenerator.generate(parser.getParseTree());
        tacGenerator.printInstructions();

        //TODO list:
        //TODO - three address code generation
            //TODO - store list of labels and their counts and increment
            //TODO - store count of conditional labels e.g. L9 for branching
        //TODO - report on three address code generation
    }

    return 0;
}
