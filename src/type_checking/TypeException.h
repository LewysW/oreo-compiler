#pragma once

#include <exception>

/**
 * Custom error which is thrown if an error
 * occurs during type checking
 */
class TypeException : public std::exception {
private:
    const char* errMsg;

public:
    explicit TypeException(const char* msg) : errMsg(msg)
    {
    }

    virtual const char* what() const throw() {
        return errMsg;
    }
};
