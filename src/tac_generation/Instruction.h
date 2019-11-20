#pragma once

#include "../parser/Pattern.h"

class Instruction {
private:
    std::string op;
    std::string arg1;
    std::string arg2;
    std::string result;

public:
    Instruction(const std::string& op, const std::string &arg1, const std::string &arg2, const std::string &result);

    const std::string& getOp() const;

    const std::string &getArg1() const;

    const std::string &getArg2() const;

    const std::string &getResult() const;
};


