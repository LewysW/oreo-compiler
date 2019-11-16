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
    //Constructor
    Semantic();

    //Performs semantic analysis
    void analyse(const std::shared_ptr<TreeNode>& parseTree);

    //Validates scope of symbols in parse tree
    void validateScope(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope);

    //Validates symbols of statements in the inner blocks of the parse tree
    void statement(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope);

    //Validates the symbols involved in a variable declaration and adds symbols to symbol table
    void variable(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope);

    //Validates the symbols involved in a print statement
    void printStmt(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope);

    //Validates the symbols involved in conditional statements and creates a new scope for each
    void conditionalStmt(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope);

    //Validates the symbols of an assignment
    void assignment(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope);

    //Validates symbols of a function signature/declaration, adds the function symbol to a symbol table and creates
    //a new scope for the function
    void functionSig(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope);

    //Validates the function call ID and actual parameter symbols
    void functionCall(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope);

    //Validates the symbols of a return statement
    void returnStmt(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope);

    //Validates the symbols of an expression
    void expression(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope);

    //Throws an error if an ID is not in scope
    void checkIDScope(const Token& token, Object obj, const std::shared_ptr<Scope>& scope);

    //Throws an error if an ID has been declared multiple times in the same scope
    void checkIDDeclaration(const Token& token, Object obj, const std::shared_ptr<Scope>& scope);

    //Prints the scope tree
    void printTree(const std::shared_ptr<Scope>& global);

    //Prints a scope, as well as its inner symbols/scopes
    void printScope(const std::shared_ptr<Scope>& scope);
};


