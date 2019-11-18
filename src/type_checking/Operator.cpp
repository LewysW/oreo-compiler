#include "Operator.h"

const std::pair<Type, Type> &Operator::getOperands() const {
    return operands;
}

Type Operator::getOutput() const {
    return output;
}

Operator::Operator(const std::pair<Type, Type> &operands, Type output) {

}
