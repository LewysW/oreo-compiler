#include <iostream>
#include "Semantic.h"
#include "SemanticException.h"

const std::shared_ptr<Scope> &Semantic::getGlobalScope() const {
    return globalScope;
}

void Semantic::analyse(const std::shared_ptr<TreeNode> &parseTree) {
    bool valid;

    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getLabel() == "Compound") {
            validateScope(parseTree, globalScope);
        }
    }
}


void Semantic::validateScope(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope) {
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getLabel() == "Statement") {
            statement(node, scope);
        }
    }
}

void Semantic::statement(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope) {
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getLabel() == "Variable") {
            variable(node, scope);
        } else if (node->getLabel() == "Print Statement") {
            printStmt(node, scope);
        } else if (node->getLabel() == "While") {
            whileStmt(node, scope);
        } else if (node->getLabel() == "If") {
            ifStmt(node, scope);
        } else if (node->getLabel() == "Assignment") {
            assignment(node, scope);
        } else if (node->getLabel() == "Function Signature") {
            functionSig(node, scope);
        } else if (node->getLabel() == "Function Call") {
            functionCall(node, scope);
        } else if (node->getLabel() == "Return") {
            returnStmt(node, scope);
        }
    }
}

void Semantic::variable(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {
    std::string id;
    Type type = Type::INT;
    unsigned long line = 0;
    unsigned long character = 0;
    bool isAssignment = false;
    std::shared_ptr<TreeNode> current;

    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getLabel() == "Variable Assignment") {
            isAssignment = true;
            current = node;
            break;
        }

        switch (node->getToken().getType()) {
            case Pattern::TokenType::INT:
                type = Type::INT;
                break;
            case Pattern::TokenType::BOOL:
                type = Type::BOOL;
                break;
            case Pattern::TokenType::STRING:
                type = Type::STRING;
                break;
            case Pattern::TokenType::ID:
                id = node->getToken().getValue();
                line = node->getToken().getLineNum();
                character = node->getToken().getColNum();
                break;
        }
    }

    if (scope->declared(id)) {
        std::string err = "Multiple declaration of variable '" + id + "' on line ";
        err += std::to_string(line) + ", character " + std::to_string(character);
        std::cout << err << std::endl;
        throw SemanticException(nullptr);
    }

    scope->addSymbol(id, Object::VAR, type);

    if (isAssignment) {
        for (const std::shared_ptr<TreeNode>& node : current->getChildren()) {
            if (node->getLabel() == "Expression") {
                expression(node, scope);
            }
        }
    }
}

void Semantic::printStmt(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {
    Pattern::TokenType token = parseTree->getChildren().at(0)->getToken().getType();

    switch (token) {
        case Pattern::TokenType::PRINT:
        case Pattern::TokenType::PRINTLN:
            for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
                if (node->getLabel() == "Expression") {
                    expression(node, scope);
                }
            }
            break;
        case Pattern::TokenType::GET:
            for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
                if (node->getToken().getType() == Pattern::TokenType::ID) {
                    std::string id = node->getToken().getValue();
                    unsigned long line = node->getToken().getLineNum();
                    unsigned long character = node->getToken().getColNum();

                    if (!scope->inScope(id)) {
                        std::string err = "Identifier '" + id + "' on line ";
                        err += std::to_string(line) + ", character " + std::to_string(character);
                        err += " not in scope";
                        std::cout << err << std::endl;
                        throw SemanticException(nullptr);
                    }
                }
            }
    }
}

void Semantic::whileStmt(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {

}

void Semantic::ifStmt(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {

}

void Semantic::assignment(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {

}

void Semantic::functionSig(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {

}

void Semantic::functionCall(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {

}

void Semantic::returnStmt(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {

}

void Semantic::expression(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {

}

