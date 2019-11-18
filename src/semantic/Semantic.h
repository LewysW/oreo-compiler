#pragma once


#include "../parser/TreeNode.h"
#include "Scope.h"

class Semantic {
private:
public:
    const std::shared_ptr<Scope> &getGlobalScope() const;

private:
    std::shared_ptr<Scope> globalScope;
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
public:
    //Constructor
    Semantic();

    //Performs semantic analysis
    void analyse(const std::shared_ptr<TreeNode>& parseTree);
    //Prints the scope tree
    void printTree(const std::shared_ptr<Scope>& global);

    //Prints a scope, as well as its inner symbols/scopes
    void printScope(const std::shared_ptr<Scope>& scope);

    //Maps labels to operator token types
    inline static const std::map<std::string, Pattern::TokenType> labelToToken {
            {"PLUS", Pattern::TokenType::PLUS},
            {"MINUS", Pattern::TokenType::MINUS},
            {"MULTIPLY", Pattern::TokenType::MULTIPLY},
            {"DIVIDE", Pattern::TokenType::DIVIDE},
            {"AND", Pattern::TokenType::AND},
            {"OR", Pattern::TokenType::OR},
            {"LT", Pattern::TokenType::LT},
            {"GT", Pattern::TokenType::GT},
            {"LTE", Pattern::TokenType::LTE},
            {"GTE", Pattern::TokenType::GTE},
            {"EQ", Pattern::TokenType::EQ},
            {"NOT", Pattern::TokenType::NOT},
    };
};


