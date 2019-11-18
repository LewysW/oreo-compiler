#include "Operator.h"

const std::pair<Type, Type> &Operator::getOperands() const {
    return operands;
}



Operator::Operator(const std::pair<Type, Type> &operands, Type output) : operands(operands), output(output) {}

Type Operator::getOutput() const {
    return output;
}


