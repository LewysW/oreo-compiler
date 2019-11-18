#include "TypeChecker.h"
#include "TypeException.h"

void TypeChecker::checkTypes(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope> global) {
    try {

    } catch (TypeException& e) {
        exit(4);
    }
}
