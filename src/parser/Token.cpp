#include "Token.h"

/**
 * Constructor for token
 * @param tok - type of token (enum)
 * @param val - value if a string, number, comment, id
 * @param line - line token was found
 * @param col - column token was found at
 */
Token::Token(Pattern::TokenType tok, std::string val, unsigned long line, unsigned long col) :
        type(tok),
        value(std::move(val)),
        lineNum(line),
        colNum(col)
{
}

/**
 * Getter for token type
 * @return type
 */
Pattern::TokenType Token::getType() const {
    return type;
}

/**
 * Getter for value
 * @return value
 */
const std::string &Token::getValue() const {
    return value;
}

/**
 * Getter for lineNum
 * @return lineNum
 */
unsigned long Token::getLineNum() const {
    return lineNum;
}

/**
 * Getter for colNum
 * @return colNum
 */
unsigned long Token::getColNum() const {
    return colNum;
}
