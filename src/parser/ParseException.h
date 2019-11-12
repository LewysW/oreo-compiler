#pragma once

#include <iostream>
#include <exception>

//Custom exception thrown if an error
// occurs during lexical or syntactic analysis
class ParseException: public std::exception {
private:
    const char* errMsg;

public:
    explicit ParseException(const char* msg) : errMsg(msg)
    {
    }

    virtual const char* what() const throw() {
        return errMsg;
    }
};