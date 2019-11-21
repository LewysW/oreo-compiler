#include "Instruction.h"

const std::string& Instruction::getOp() const {
    return op;
}

const std::string &Instruction::getArg1() const {
    return arg1;
}

const std::string &Instruction::getArg2() const {
    return arg2;
}

const std::string &Instruction::getResult() const {
    return result;
}

Instruction::Instruction(const std::string& op, const std::string &arg1, const std::string &arg2,
                         const std::string &result) : op(op), arg1(arg1), arg2(arg2), result(result), label(std::string("")) {}

const std::string &Instruction::getLabel() const {
    return label;
}

void Instruction::setLabel(const std::string &label) {
    Instruction::label = label;
}

bool Instruction::isBranchInstruction() const {
    return branchInstruction;
}

void Instruction::setBranchInstruction(bool branchInstruction) {
    Instruction::branchInstruction = branchInstruction;
}
