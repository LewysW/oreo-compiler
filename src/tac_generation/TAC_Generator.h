#pragma once
#include <iostream>
#include <queue>
#include <map>
#include "Instruction.h"
#include "../parser/TreeNode.h"

class TAC_Generator {
private:
    //Stores list of generated three address code instructions
    std::vector<Instruction> instructions;
    //Stores number of temporary variable names
    unsigned long tempIDNum = 1;
    //Stores number of conditional labels
    unsigned long labelNum = 0;

    bool startOfBlock = false;
    std::string blockLabel;

    void scope(const std::shared_ptr<TreeNode>& parseTree);
    void statement(const std::shared_ptr<TreeNode>& parseTree);
    void variable(const std::shared_ptr<TreeNode>& parseTree);
    void printStmt(const std::shared_ptr<TreeNode>& parseTree);
    void conditionalStmt(const std::shared_ptr<TreeNode>& parseTree);
    std::string expression(const std::shared_ptr<TreeNode>& parseTree);
    static bool isOperation(const std::string& label);
    std::string addInstruction(std::string op, std::string arg1, std::string arg2, std::string result);

public:
    std::string getNextID();
    std::string getNextLabel();
    void generate(const std::shared_ptr<TreeNode>& parseTree);
    void printInstructions();

    bool isStartOfBlock() const;

    void setStartOfBlock(bool startOfBlock);

    const std::string &getBlockLabel() const;

    void setBlockLabel(const std::string &blockLabel);

};
