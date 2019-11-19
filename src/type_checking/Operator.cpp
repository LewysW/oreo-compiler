#include "Operator.h"

/**
 * Getter for operator operands
 * @return pair of operands
 */
const std::pair<Type, Type> &Operator::getOperands() const {
    return operands;
}

/**
 * Constructor for operator
 * @param operands - of operator
 * @param output - result of operator
 */
Operator::Operator(const std::pair<Type, Type> &operands, Type output) : operands(operands), output(output) {}

/**
 * Getter for output
 * @return output type
 */
Type Operator::getOutput() const {
    return output;
}


