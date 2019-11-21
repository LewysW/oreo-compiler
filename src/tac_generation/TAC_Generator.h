#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include "Instruction.h"
#include "../parser/TreeNode.h"

/**
 * Class to generate a list of three address code
 * instructions from semantically correct parse tree
 */
class TAC_Generator {
private:
    //Stores list of generated three address code instructions
    std::vector<Instruction> instructions;
public:
    //Getter for list of instructions
    const std::vector<Instruction> &getInstructions() const;

private:

    //Stores number of temporary variable names (counts from 1, as in the example file)
    unsigned long tempIDNum = 1;
    //Stores number of conditional labels (counts from 0, as in the example file)
    unsigned long labelNum = 0;

    //Indicates whether the next added instruction requires a label
    bool labelRequired = false;
    //Stores label to add to instruction
    std::string blockLabel;

    //Translates the current scope into three address code
    void scope(const std::shared_ptr<TreeNode>& parseTree);

    //Translates the current statement into three address code
    void statement(const std::shared_ptr<TreeNode>& parseTree);

    //Translates the current variable into three address code
    void variable(const std::shared_ptr<TreeNode>& parseTree);

    //Translates the current print statement int three address code
    void printStmt(const std::shared_ptr<TreeNode>& parseTree);

    //Translates the current if/else statement into three address code
    void ifStmt(const std::shared_ptr<TreeNode>& parseTree);

    //Translates the current while loop into three address code
    void whileLoop(const std::shared_ptr<TreeNode>& parseTree);

    //Translates the current assignment into three address code
    void assignment(const std::shared_ptr<TreeNode>& parseTree);

    //Translates the current function declaration/signature into three address code
    void functionSig(const std::shared_ptr<TreeNode>& parseTree);

    //Translates the current function into three address code
    std::string functionCall(const std::shared_ptr<TreeNode>& parseTree);

    //Translates the current return statement into three address code
    void returnStmt(const std::shared_ptr<TreeNode>& parseTree);

    //Translates the current expression into three address code
    std::string expression(const std::shared_ptr<TreeNode>& parseTree);

    //returns whether a given label is an operation (e.g. relational or arithmetic)
    static bool isOperation(const std::string& label);

    //Adds an instruction to the list of instructions given a quadruple
    std::string addInstruction(std::string op, std::string arg1, std::string arg2, std::string result);

    //Prints a given instruction
    void printInstruction(Instruction instruction);

public:
    //Getter for next temporary ID value
    std::string getNextID();

    //Getter for next label value
    std::string getNextLabel();

    //Generates a list of three address code instructions from a given parse tree
    void generate(const std::shared_ptr<TreeNode>& parseTree);

    //Prints the list of instructions
    void printInstructions();

    //Returns whether current instruction needs a label prepended
    bool isLabelRequired() const;

    //Getter the number for the next label to add
    unsigned long getLabelNum() const;

    //Sets whether the next instruction requires a label prepended
    void setLabelRequired(bool labelRequired);

    //Gets value of next label to prepend
    const std::string &getBlockLabel() const;

    //Sets value of next label to prepend
    void setBlockLabel(const std::string &blockLabel);

};
