#pragma once

#include "Pattern.h"
#include "ParseException.h"
#include "Token.h"
#include <string>
#include <map>
#include <vector>
#include <regex>
#include <iostream>
#include <fstream>

class Lexer {
private:
    //Content of file to analyse
    std::string fileContent;
    //list to store tokens
    std::vector<Token> tokenList;
public:
    //Constructor
    explicit Lexer(std::string const& file);

    //Prints the list of tokens
    void printTokens();

    //Allows external printing of tokens, passing in a list
    static void printTokens(const std::vector<Token>& tokens);

    //Reads the contents of a file into a string
    std::string readFile(std::string const& file) const;

    //Tokenizes the file contents into a list of tokens
    std::vector<Token> tokenize() const;

    //Checks if a string is a comment
    Pattern::TokenType isComment(std::string &s, unsigned long &tokLen, unsigned long &lineCount) const;

    //Checks if a string is a string literal
    Pattern::TokenType isStrLiteral(std::string &s, unsigned long &tokLen) const;

    //Checks if a string is an operator or whitespace
    Pattern::TokenType isOperator(std::string &stream, unsigned long &tokLen) const;

    //Checks if a string is a keyword
    Pattern::TokenType isKeyword(std::string &s, unsigned long &tokLen,
                                 std::map<std::string, Pattern::TokenType> const &keywords) const;

    //Checks if a string is a number
    Pattern::TokenType isNumeric(std::string &s, unsigned long &tokLen) const;

    //Checks if a string is an identifier
    Pattern::TokenType isIdentifier(std::string &s, unsigned long &tokLen) const;

    //Getter for keywords
    std::map<std::string, Pattern::TokenType> getKeywords() const;

    //Returns the lexeme up until the next operator, comment, or string literal
    std::string buffer(std::string& stream) const;

    //Getter for file content
    const std::string &getFileContent() const;

    //Getter for token list
    const std::vector<Token> &getTokenList() const;

    //Allows token symbols to be printed by using the TokenType
    // as the index to the following list of strings:
    inline static const std::vector<std::string> TOKEN_STRINGS {
            "NONE",
            "<", //Less than
            "<=", //Less than equal to
            ">", //Greater than
            ">=", //Greater than equal to
            "==", //Equal to
            ":=", //assignment
            ";", //Semi colon
            "+", // +
            "-", // -
            "/", // '/'
            "*", // *
            "STRING", //String literal
            "(", //Left paren
            ")", //Right paren
            "COMMENT", //Comment
            ",", // comma
            "begin", //begin
            "end", //end
            "program", //program
            "procedure", //procedure
            "return", //return
            "if", //if
            "then", // then
            "else", //else
            "while", //while
            "print", //print
            "println", //println
            "var", // var
            "get", // get
            "and", // and
            "or", // or
            "not", // not
            "true", // true
            "false", // false
            "NEWLINE", //\n
            "WHITESPACE", //\r \s \t
            "NUM", //sequence of digits
            "ID" //Identifier
    };
};

