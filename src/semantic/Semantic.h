#pragma once


#include "../parser/TreeNode.h"
#include "Scope.h"

class Semantic {
private:
    Scope globalScope;
public:
    const Scope &getGlobalScope() const;

public:
    Semantic() = default;

    void analyse(const std::shared_ptr<TreeNode>& parseTree);
};


