#pragma once

#include <exception>

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
