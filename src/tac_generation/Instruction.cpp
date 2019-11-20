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
                         const std::string &result) : op(op), arg1(arg1), arg2(arg2), result(result) {}
