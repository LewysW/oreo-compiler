#include <iostream>
#include "Semantic.h"
#include "SemanticException.h"

const std::shared_ptr<Scope> &Semantic::getGlobalScope() const {
    return globalScope;
}

void Semantic::analyse(const std::shared_ptr<TreeNode> &parseTree) {
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getLabel() == "Compound") {
            validateScope(node, globalScope);
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
        std::string label = node->getLabel();

        if (label == "Variable") {
            variable(node, scope);
        } else if (label == "Print Statement") {
            printStmt(node, scope);
        } else if (label == "While" || label == "If" || label == "Else") {
            conditionalStmt(node, scope);
        } else if (label == "Assignment") {
            assignment(node, scope);
        } else if (label == "Function Signature") {
            functionSig(node, scope);
        } else if (label == "Function Call") {
            functionCall(node, scope);
        } else if (label == "Return") {
            returnStmt(node, scope);
        }
    }
}

void Semantic::variable(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {
    std::string id;
    Token token = parseTree->getToken();
    Type type = Type::INT;
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
                token = node->getToken();
                id = node->getToken().getValue();
                break;
        }
    }

    checkIDDeclaration(token, Object::VAR, scope);

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
                    checkIDScope(node->getToken(), Object::VAR, scope);
                }
            }
    }
}

void Semantic::conditionalStmt(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {
    Block block;

    if (parseTree->getLabel() == "While") {
        block = Block::WHILE;
    } else if (parseTree->getLabel() == "If") {
        block = Block::IF;
    } else if (parseTree->getLabel() == "Else") {
        block = Block::ELSE;
    }

    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getLabel() == "Expression") {
            expression(node, scope);
        } else if (node->getLabel() == "Compound") {

            scope->addScope(block);
            validateScope(node, std::make_shared<Scope>(scope->getScopes().back()->second));
        }
    }
}


void Semantic::assignment(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getToken().getType() == Pattern::TokenType::ID) {
            checkIDScope(node->getToken(), Object::VAR, scope);
        } else if (node->getLabel() == "Expression") {
            expression(node, scope);
        }
    }
}

void Semantic::functionSig(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {
    Type type;

    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        //Checks if the procedure has been declared previously
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
                checkIDDeclaration(node->getToken(), Object::PROC, scope);
                scope->addSymbol(node->getToken().getValue(), Object::PROC, type);
                scope->addScope(Block::PROC);
                break;
        }
    }

    std::shared_ptr<Scope> procScope = std::make_shared(scope->getScopes().back()->second);

    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getLabel() == "Formal Parameter") {
            variable(node, procScope);
        } else if (node->getLabel() == "Compound") {
            validateScope(node, procScope);
        }
    }
}


void Semantic::functionCall(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {
    
}

void Semantic::returnStmt(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {

}

void Semantic::expression(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {

}

void Semantic::checkIDScope(const Token& token, const Object obj, const std::shared_ptr<Scope>& scope) {
    const std::string& id = token.getValue();
    unsigned long line = token.getLineNum();
    unsigned long character = token.getColNum();

    if (!scope->inScope(id, obj)) {
        std::string err = "Identifier '" + id + "' on line ";
        err += std::to_string(line) + ", character " + std::to_string(character);
        err += " not in scope";
        std::cout << err << std::endl;
        throw SemanticException(nullptr);
    }
}

void Semantic::checkIDDeclaration(const Token& token, const Object obj, const std::shared_ptr<Scope>& scope) {
    const std::string& id = token.getValue();
    unsigned long line = token.getLineNum();
    unsigned long character = token.getColNum();

    if (scope->declared(id)) {
        std::string err = "Multiple declaration of ";
        err += (obj == Object::VAR) ? "variable" : "procedure";
        err += " " + id;
        err += " on line " + std::to_string(line) + ", character " + std::to_string(character);
        std::cout << err << std::endl;
        throw SemanticException(nullptr);
    }
}



