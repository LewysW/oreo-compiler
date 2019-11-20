#pragma once

#include "../parser/Pattern.h"

class Instruction {
private:
    Pattern::TokenType op;
    std::string arg1;
    std::string arg2;
    std::string result;

public:
    Instruction(Pattern::TokenType op, const std::string &arg1, const std::string &arg2, const std::string &result);

    Pattern::TokenType getOp() const;

    const std::string &getArg1() const;

    const std::string &getArg2() const;

    const std::string &getResult() const;
};


