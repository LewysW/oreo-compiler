#include "Semantic.h"

const Scope &Semantic::getGlobalScope() const {
    return globalScope;
}

void Semantic::analyse(const std::shared_ptr<TreeNode> &parseTree) {
    
}
