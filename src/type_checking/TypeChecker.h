#pragma once


#include "../parser/TreeNode.h"
#include "../semantic/Scope.h"
#include "../semantic/Semantic.h"
#include "Operator.h"

class TypeChecker {
private:

public:
    void checkTypes(const std::shared_ptr<TreeNode>& parseTree, const std::shared_ptr<Scope> global);

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
            {Pattern::TokenType::NOT, Operator({Type::NONE, Type::INT}, Type::INT)},

    };
};


