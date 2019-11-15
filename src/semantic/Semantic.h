#pragma once


#include "../parser/TreeNode.h"
#include "Scope.h"

class Semantic {
private:
public:
    const std::shared_ptr<Scope> &getGlobalScope() const;

private:
    std::shared_ptr<Scope> globalScope;
public:


public:
    Semantic();

    void analyse(const std::shared_ptr<TreeNode>& parseTree);

    void validateScope(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope);

    void statement(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope);

    void variable(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope);

    void printStmt(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope);

    void conditionalStmt(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope);

    void assignment(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope);

    void functionSig(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope);

    void functionCall(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope);

    void returnStmt(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope);

    void expression(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope);

    void checkIDScope(const Token& token, Object obj, const std::shared_ptr<Scope>& scope);

    void checkIDDeclaration(const Token& token, Object obj, const std::shared_ptr<Scope>& scope);

    void print(const std::shared_ptr<Scope> scope);
};


