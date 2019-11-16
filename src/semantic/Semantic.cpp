#include <iostream>
#include "Semantic.h"
#include "SemanticException.h"

/**
 * Constructor for Semantic class, initialises the
 * globalScope variable with a new Scope shared_ptr
 */
Semantic::Semantic() : globalScope(std::make_shared<Scope>(Scope()))
{
}

/**
 * Getter for globalScope, the root of a scope tree
 * @return globalScope scope pointer
 */
const std::shared_ptr<Scope> &Semantic::getGlobalScope() const {
    return globalScope;
}

/**
 * Analyses a given parse tree's semantics,
 * populating a tree of scopes and their symbol tables if valid,
 * throwing an error if not
 * @param parseTree - to semantically analyse
 */
void Semantic::analyse(const std::shared_ptr<TreeNode> &parseTree) {
    //Try to generate tree of scopes and symbol tables
    try {
        for (const std::shared_ptr<TreeNode> &node : parseTree->getChildren()) {
            //Validate Compound of global scope
            if (node->getLabel() == "Compound") {
                validateScope(node, globalScope);
            }
        }
    //If invalid, catches exception and exits
    } catch (SemanticException& e) {
        exit(3);
    }
}

/**
 * Validates the statem
 * @param parseTree
 * @param scope
 */
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
            validateScope(node, scope->getScopes().back());
        } else if (node->getLabel() == "Else") {
            conditionalStmt(node, scope);
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

    std::shared_ptr<Scope> procScope = scope->getScopes().back();

    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getLabel() == "Formal Parameter") {
            variable(node, procScope);
        } else if (node->getLabel() == "Compound") {
            validateScope(node, procScope);
        }
    }
}


void Semantic::functionCall(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        switch (node->getToken().getType()) {
            case Pattern::TokenType::ID:
                checkIDScope(node->getToken(), Object::PROC, scope);
                break;
            default:
                break;
        }

        if (node->getLabel() == "Actual Parameter") {
            expression(node, scope);
        }
    }
}

void Semantic::returnStmt(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getLabel() == "Expression") {
            expression(node, scope);
            break;
        }
    }
}

void Semantic::expression(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getLabel() == "Function Call") {
            functionCall(node, scope);
        } else if (node->getLabel() == "Expression") {
            expression(node, scope);
        }

        switch (node->getToken().getType()) {
            case Pattern::TokenType::ID:
                checkIDScope(node->getToken(), Object::VAR, scope);
                break;
            default:
                break;
        }
    }
}

void Semantic::checkIDScope(const Token& token, const Object obj, const std::shared_ptr<Scope>& scope) {
    const std::string& id = token.getValue();
    unsigned long line = token.getLineNum();
    unsigned long character = token.getColNum();
    if (!scope->inScope(id, obj)) {
        std::string err = (obj == Object::VAR) ? "Variable \'" : "Procedure \'";
        err += id + "' on line ";
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

void Semantic::printTree(const std::shared_ptr<Scope>& global) {
    std::cout << "Semantic Analysis:" << std::endl;
    std::cout << "------------------------------------------------------------------" << std::endl;
    printScope(global);
}

void Semantic::printScope(const std::shared_ptr<Scope>& scope) {
    //Stores number of tabs and commas to print
    static std::string tabStr;

    switch (scope->getBlock()) {
        case Block::GLOBAL:
            std::cout << tabStr << "Global: ";
            break;
        case Block::PROC:
            std::cout << tabStr << "Procedure: ";
            break;
        case Block::IF:
        std::cout << tabStr << "If: ";
            break;
        case Block::ELSE:
            std::cout << tabStr << "Else: ";
            break;
        case Block::WHILE:
            std::cout << tabStr <<"While: ";
            break;
    }

    tabStr += "\t";

    std:: cout << "{" << std::endl;

    for (std::pair<std::string, std::pair<Object, Type>> symbol : scope->getSymbolTable()) {
        std::cout << tabStr << "[\"ID\":" << symbol.first << ", ";

        switch (symbol.second.first) {
            case Object::VAR:
                std::cout << "var, ";
                break;
            case Object::PROC:
                std::cout << "proc, ";
                break;
        }

        switch (symbol.second.second) {
            case Type::INT:
                std::cout << "int";
                break;
            case Type::BOOL:
                std::cout << "bool";
                break;
            case Type::STRING:
                std::cout << "string";
                break;
        }

        std::cout << "]" << std::endl;
    }


    for (std::shared_ptr<Scope> s : scope->getScopes()) {
        std::cout << std::endl;
        printScope(s);
    }

    if (tabStr.length() > 0) {
        tabStr.erase(tabStr.length() - 1);
    }

    std::cout << tabStr << "}" << std::endl;
}



