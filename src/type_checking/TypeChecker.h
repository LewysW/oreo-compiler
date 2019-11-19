#pragma once


#include "../parser/TreeNode.h"
#include "../semantic/Scope.h"
#include "../semantic/Semantic.h"
#include "Operator.h"

class TypeChecker {
public:
    //Main type checking logic
    void checkTypes(const std::shared_ptr<TreeNode>& parseTree, const std::shared_ptr<Scope>& global);
private:

    //Validates types of a scope in parse tree
    void validateScopeTypes(const std::shared_ptr<TreeNode>& parseTree, const std::shared_ptr<Scope>& scope);

    //Validates types of statements in the inner blocks of the parse tree
    void statement(const std::shared_ptr<TreeNode>& parseTree, const std::shared_ptr<Scope>& scope);

    //Validates the types involved in a variable declaration
    void variable(const std::shared_ptr<TreeNode>& parseTree, const std::shared_ptr<Scope>& scope);

    //Validates the types involved in a print statement
    void printStmt(const std::shared_ptr<TreeNode>& parseTree, const std::shared_ptr<Scope>& scope);

    //Validates the types involved in conditional statements
    void conditionalStmt(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope>& scope);

    //Validates the types of an assignment
    void assignment(const std::shared_ptr<TreeNode>& parseTree, const std::shared_ptr<Scope>& scope);

    //Validates types of a function signature/declaration
    void functionSig(const std::shared_ptr<TreeNode>& parseTree, const std::shared_ptr<Scope>& scope);

    //Validates the function call actual parameter types and returns the type of function
    Type functionCall(const std::shared_ptr<TreeNode>& parseTree, const std::shared_ptr<Scope>& scope);

    //Validates the types of a return statement
    void returnStmt(const std::shared_ptr<TreeNode>& parseTree, const std::shared_ptr<Scope>& scope);

    //Validates the types of an expression
    void expression(const std::shared_ptr<TreeNode>& parseTree, const std::shared_ptr<Scope>& scope, Type expected, unsigned long line);

    Type evaluateExpression(const std::shared_ptr<TreeNode>& parseTree, const std::shared_ptr<Scope>& scope, unsigned long line);;

    void generateTypeError(Type expected, Type result, unsigned long lineNum);
    void generateOperatorError(Pattern::TokenType type, Type op1, Type op2, unsigned long lineNum);

    inline static const std::map<Pattern::TokenType, Operator> operators {
            {Pattern::TokenType::PLUS, Operator({Type::INT, Type::INT}, Type::INT)},
            {Pattern::TokenType::MINUS, Operator({Type::INT, Type::INT}, Type::INT)},
            {Pattern::TokenType::MULTIPLY, Operator({Type::INT, Type::INT}, Type::INT)},
            {Pattern::TokenType::DIVIDE, Operator({Type::INT, Type::INT}, Type::INT)},
            {Pattern::TokenType::AND, Operator({Type::BOOL, Type::BOOL}, Type::BOOL)},
            {Pattern::TokenType::OR, Operator({Type::BOOL, Type::BOOL}, Type::BOOL)},
            {Pattern::TokenType::LT, Operator({Type::INT, Type::INT}, Type::BOOL)},
            {Pattern::TokenType::GT, Operator({Type::INT, Type::INT}, Type::BOOL)},
            {Pattern::TokenType::LTE, Operator({Type::INT, Type::INT}, Type::BOOL)},
            {Pattern::TokenType::GTE, Operator({Type::INT, Type::INT}, Type::BOOL)},
            {Pattern::TokenType::EQ, Operator({Type::INT, Type::INT}, Type::BOOL)},
            {Pattern::TokenType::NOT, Operator({Type::NONE, Type::BOOL}, Type::BOOL)},

    };
};


