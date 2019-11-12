#pragma once

#include <string>
#include "Pattern.h"

/**
 * Representation of token parse from file
 */
class Token {
private:
    //Type of token (enum)
    Pattern::TokenType type;
    //Value of token if a string, id, comment, or num
    std::string value;
    //line and column number encountered at
    unsigned long lineNum;
    unsigned long colNum;

public:
    //Constructor
    Token(Pattern::TokenType tok, std::string val, unsigned long line, unsigned long col);

    //Getter for type
    Pattern::TokenType getType() const;

    //Getter for value
    const std::string &getValue() const;

    //Getter for lineNum
    unsigned long getLineNum() const;

    //Getter for colNum
    unsigned long getColNum() const;
};


