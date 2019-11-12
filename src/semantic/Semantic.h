#pragma once


#include "../parser/TreeNode.h"

class Semantic {
public:
    Semantic::Semantic() = default;
    void analyse(const std::shared_ptr<TreeNode>& parseTree);
};


