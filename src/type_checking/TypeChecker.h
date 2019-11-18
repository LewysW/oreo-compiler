#pragma once


#include "../parser/TreeNode.h"
#include "../semantic/Scope.h"

class TypeChecker {
public:
    void checkTypes(const std::shared_ptr<TreeNode>& parseTree, const std::shared_ptr<Scope> global);
private:

};


