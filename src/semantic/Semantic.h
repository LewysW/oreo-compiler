#pragma once


#include "../parser/TreeNode.h"

class Semantic {
public:
    Semantic(const std::shared_ptr<TreeNode>& parseTree);

    void analyse(const std::shared_ptr<TreeNode>& parseTree);
};


