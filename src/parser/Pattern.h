#pragma once

#include <string>
#include <vector>

/**
 * Pattern namespace defines an enum class of TokenTypes
 */
namespace Pattern {
    enum class TokenType {
        NONE = 0,
        LT,
        LTE,
        GT,
        GTE,
        EQ,
        ASSIGN,
        SEMI,
        PLUS,
        MINUS,
        DIVIDE,
        MULTIPLY,
        STRING,
        LPAREN,
        RPAREN,
        COMMENT,
        COMMA,
        BEGIN,
        END,
        PROGRAM,
        PROCEDURE,
        RETURN,
        IF,
        THEN,
        ELSE,
        WHILE,
        PRINT,
        PRINTLN,
        VAR,
        GET,
        AND,
        OR,
        NOT,
        TRUE,
        FALSE,
        NEWLINE,
        WHITESPACE,
        NUM,
        ID
    };

}

