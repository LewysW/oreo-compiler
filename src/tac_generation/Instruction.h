#pragma once

#include "../parser/Pattern.h"

/**
 * Models a single three address code instruction (as a quadruple)
 */
class Instruction {
private:
    //quadruple:

    //Stores operation of instruction
    std::string op;
    //Stores first argument of instruction
    std::string arg1;
    //Stores second argument of instruction
    std::string arg2;
    //Stores result of instruction
    std::string result;

    //Optional label for conditional branching
    std::string label;

    //Whether instruction involves branching
    bool branchInstruction = false;
public:
    //Constructor for instruction
    Instruction(const std::string& op, const std::string &arg1, const std::string &arg2, const std::string &result);

    //Getter for operation
    const std::string& getOp() const;

    //Getter for arg1
    const std::string &getArg1() const;

    //Getter for arg2
    const std::string &getArg2() const;

    //Getter for result
    const std::string &getResult() const;

    //Getter for label
    const std::string &getLabel() const;

    //Setter for label
    void setLabel(const std::string &label);

    //Returns whether instruction is branch instruction
    bool isBranchInstruction() const;

    //Setter for branchInstruction
    void setBranchInstruction(bool branchInstruction);


};


