#include <iostream>
#include <map>
#include <regex>
#include "parser/Lexer.h"
#include "parser/Parser.h"

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
    }

    return 0;
}
