#pragma once

#include "../semantic/Scope.h"

class Operator {
private:
    std::pair<Type, Type> operands;
    Type output;
public:
    Operator(const std::pair<Type, Type> &operands, Type output);

    const std::pair<Type, Type> &getOperands() const;

    Type getOutput() const;
};

Operator::Operator(const std::pair<Type, Type> &operands, Type output) : operands(operands), output(output) {}


