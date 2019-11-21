#include "Instruction.h"

/**
 * Getter for operation
 * @return operation
 */
const std::string& Instruction::getOp() const {
    return op;
}

/**
 * Getter for arg1
 * @return arg1
 */
const std::string &Instruction::getArg1() const {
    return arg1;
}

/**
 * Getter for arg2
 * @return arg2
 */
const std::string &Instruction::getArg2() const {
    return arg2;
}

/**
 * Getter for result
 * @return result
 */
const std::string &Instruction::getResult() const {
    return result;
}

/**
 * Constructor for Instruction
 * @param op - operation of instruction
 * @param arg1 - first argument of instruction
 * @param arg2 - second argument of instruction
 * @param result - result of instruction
 */
Instruction::Instruction(const std::string& op, const std::string &arg1, const std::string &arg2,
                         const std::string &result) : op(op), arg1(arg1), arg2(arg2), result(result), label(std::string("")) {}

/**
 * Getter for label
 * @return label
 */
const std::string &Instruction::getLabel() const {
    return label;
}

/**
 * Setter for label
 * @param label
 */
void Instruction::setLabel(const std::string &label) {
    Instruction::label = label;
}

/**
 * Getter for branchInstruction
 * @return whether instruction is branch instruction or not
 */
bool Instruction::isBranchInstruction() const {
    return branchInstruction;
}

/**
 * Setter for branchInstruction
 * @param branchInstruction
 */
void Instruction::setBranchInstruction(bool branchInstruction) {
    Instruction::branchInstruction = branchInstruction;
}
