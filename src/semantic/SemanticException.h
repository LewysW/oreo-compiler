#ifndef CS4201_P2_COMPILER_SEMANTICEXCEPTION_H
#define CS4201_P2_COMPILER_SEMANTICEXCEPTION_H

#pragma once

#include <iostream>
#include <exception>

//Custom exception thrown if an error
// occurs during lexical or semantic analysis
class SemanticException: public std::exception {
private:
    const char* errMsg;

public:
    explicit SemanticException(const char* msg) : errMsg(msg)
    {
    }

    virtual const char* what() const throw() {
        return errMsg;
    }
};

#endif //CS4201_P2_COMPILER_SEMANTICEXCEPTION_H
