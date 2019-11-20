#pragma once
#include <iostream>
#include <queue>
#include "Instruction.h"
#include "../parser/TreeNode.h"

class TAC_Generator {
private:
    std::vector<Instruction> instructions;
    unsigned long tempIDNum = 1;
    void scope(const std::shared_ptr<TreeNode>& parseTree);
    void statement(const std::shared_ptr<TreeNode>& parseTree);
    void variable(const std::shared_ptr<TreeNode>& parseTree);
    std::string expression(const std::shared_ptr<TreeNode>& parseTree);
    static bool isOperation(const std::string& label);

public:
    std::string getNextID();
    void generate(const std::shared_ptr<TreeNode>& parseTree);
    void printInstructions();
};
