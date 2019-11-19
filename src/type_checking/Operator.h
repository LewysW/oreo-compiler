#pragma once

#include "../semantic/Scope.h"

/**
 * Represents an operator (e.g. plus, minus, times, divide)
 * storing the required operand types and output type
 */
class Operator {
private:
    //Operands required by operator
    std::pair<Type, Type> operands;
    //Output of operator
    Type output;
public:
    //Constructor
    Operator(const std::pair<Type, Type> &operands, Type output);

    //Default constructor
    Operator() = default;

    //Getter for operands
    const std::pair<Type, Type> &getOperands() const;

    //Getter for output
    Type getOutput() const;
};


