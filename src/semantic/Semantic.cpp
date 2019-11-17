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
 * Validates the statements of each scope
 * @param parseTree - to validate
 * @param scope - to populate with symbols
 */
void Semantic::validateScope(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope) {
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getLabel() == "Statement") {
            statement(node, scope);
        }
    }
}

/**
 * Identifies the statement to be validated
 * @param parseTree - to validate
 * @param scope - to populate with symbols
 */
void Semantic::statement(const std::shared_ptr<TreeNode>& parseTree, std::shared_ptr<Scope> scope) {
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        std::string label = node->getLabel();

        //Identifies the appropriate statement type to be validated
        //and calls the appropriate function
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

/**
 * Adds a declared variable to the symbol table of
 * the current scope if not declared
 * @param parseTree - to validate
 * @param scope - to populate with symbols
 */
void Semantic::variable(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {
    std::string id;
    Token token = parseTree->getToken();
    Type type = Type::INT;
    bool isAssignment = false;
    std::shared_ptr<TreeNode> current;

    //Iterates through each symbol in the variable statement
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        //Checks if variable is being assigned a value
        if (node->getLabel() == "Variable Assignment") {
            isAssignment = true;
            current = node;
            break;
        }

        //Stores type and ID of variable
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

    //Checks if variable ID is already declared, throwing an error if it is
    checkIDDeclaration(token, Object::VAR, scope);

    //Adds the variable, its ID and type to the symbol table
    scope->addSymbol(id, Object::VAR, type);

    //If the statement was a variable declaration and assignment
    if (isAssignment) {
        for (const std::shared_ptr<TreeNode>& node : current->getChildren()) {
            //Evaluate the symbols in the expression of the assignment
            if (node->getLabel() == "Expression") {
                expression(node, scope);
            }
        }
    }
}

/**
 * Identifies the type of print statement to be validated
 * @param parseTree - to be validated
 * @param scope - to populate with symbols
 */
void Semantic::printStmt(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {
    Pattern::TokenType token = parseTree->getChildren().at(0)->getToken().getType();

    switch (token) {
        //If the print statement is print or println, then the inner expressions is validated
        case Pattern::TokenType::PRINT:
        case Pattern::TokenType::PRINTLN:
            for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
                if (node->getLabel() == "Expression") {
                    expression(node, scope);
                }
            }
            break;
        //If the print statement is get, then the scope of the following ID is validated
        case Pattern::TokenType::GET:
            for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
                if (node->getToken().getType() == Pattern::TokenType::ID) {
                    checkIDScope(node->getToken(), Object::VAR, scope);
                }
            }
    }
}

/**
 * Identifies the type of conditional statement to be validated
 * @param parseTree - to be validated
 * @param scope - to be populated with symbols
 */
void Semantic::conditionalStmt(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {
    Block block;

    //Stores the appropriate block type
    if (parseTree->getLabel() == "While") {
        block = Block::WHILE;
    } else if (parseTree->getLabel() == "If") {
        block = Block::IF;
    } else if (parseTree->getLabel() == "Else") {
        block = Block::ELSE;
    }

    //Identifies the correct part of the conditional statement to validate
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getLabel() == "Expression") {
            expression(node, scope);
        } else if (node->getLabel() == "Compound") {
            //Adds a new scope for each compound label encountered
            scope->addScope(block);
            validateScope(node, scope->getScopes().back());
        } else if (node->getLabel() == "Else") {
            conditionalStmt(node, scope);
        }
    }
}

/**
 * Validates an assignment statement
 * @param parseTree - to be validated
 * @param scope - to be populated with symbols
 */
void Semantic::assignment(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getToken().getType() == Pattern::TokenType::ID) {
            //Ensures that the ID being assigned is in scope
            checkIDScope(node->getToken(), Object::VAR, scope);
        } else if (node->getLabel() == "Expression") {
            //Validates the expression after the := sign
            expression(node, scope);
        }
    }
}

/**
 * Validates a function signature
 * @param parseTree
 * @param scope
 */
void Semantic::functionSig(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {
    Type type;

    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        //Records the return type of the function
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
                //Checks if the ID of the function has already been declared in this scope
                checkIDDeclaration(node->getToken(), Object::PROC, scope);
                //If not previously declared, procedure symbol is added to symbol table
                scope->addSymbol(node->getToken().getValue(), Object::PROC, type);
                //New scope for funciton is also created
                scope->addScope(Block::PROC);
                break;
        }
    }

    //Gets newly created procedure scope
    std::shared_ptr<Scope> procScope = scope->getScopes().back();

    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        //Validated any parameters of the procedure
        if (node->getLabel() == "Formal Parameter") {
            variable(node, procScope);
        //Validates the scope of the inner block of the function
        } else if (node->getLabel() == "Compound") {
            validateScope(node, procScope);
        }
    }
}

/**
 * Validates a procedure call
 * @param parseTree - to validate
 * @param scope - to populate with symbols
 */
void Semantic::functionCall(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        switch (node->getToken().getType()) {
            //Ensure that the called procedure ID is in scope
            case Pattern::TokenType::ID:
                checkIDScope(node->getToken(), Object::PROC, scope);
                break;
            default:
                break;
        }

        //Validate the expression of an actual parameter
        if (node->getLabel() == "Actual Parameter") {
            expression(node, scope);
        }
    }
}

/**
 * Validates a return statement
 * @param parseTree - to validate
 * @param scope - to populate with symbols
 */
void Semantic::returnStmt(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        //Validates the expression of a return statement
        if (node->getLabel() == "Expression") {
            expression(node, scope);
            break;
        }
    }
}

/**
 * Validates an expression
 * @param parseTree - to be validated
 * @param scope - to populate with symbols
 */
void Semantic::expression(const std::shared_ptr<TreeNode> &parseTree, std::shared_ptr<Scope> scope) {
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        //Validates a function call within an expression
        if (node->getLabel() == "Function Call") {
            functionCall(node, scope);
        //Recursively validates a subexpression
        } else if (node->getLabel() == "Expression") {
            expression(node, scope);
        }

        switch (node->getToken().getType()) {
            //Ensures that the IDs of variables within an expression are in scope
            case Pattern::TokenType::ID:
                checkIDScope(node->getToken(), Object::VAR, scope);
                break;
            default:
                break;
        }
    }
}

/**
 * Checks if an ID is in scope, throwing an error if not
 * @param token - to validate ID
 * @param obj - type of object (variable or function)
 * @param scope - to check symbol table of
 */
void Semantic::checkIDScope(const Token& token, const Object obj, const std::shared_ptr<Scope>& scope) {
    const std::string& id = token.getValue();
    unsigned long line = token.getLineNum();
    unsigned long character = token.getColNum();

    //If variable is not in scope, print error message and throw error
    if (!scope->inScope(id, obj)) {
        std::string err = (obj == Object::VAR) ? "Variable \'" : "Procedure \'";
        err += id + "' on line ";
        err += std::to_string(line) + ", character " + std::to_string(character);
        err += " not in scope";
        std::cout << err << std::endl;
        throw SemanticException(nullptr);
    }
}

/**
 * Checks if an ID has been previously declared,
 * throwing an error if it has
 * @param token - to validate ID
 * @param obj - type of object (variable or function)
 * @param scope - to check symbol table of
 */
void Semantic::checkIDDeclaration(const Token& token, const Object obj, const std::shared_ptr<Scope>& scope) {
    const std::string& id = token.getValue();
    unsigned long line = token.getLineNum();
    unsigned long character = token.getColNum();

    //If variable has been declared
    if (scope->declared(id, obj)) {
        std::string err = "Multiple declaration of ";
        err += (obj == Object::VAR) ? "variable" : "procedure";
        err += " " + id;
        err += " on line " + std::to_string(line) + ", character " + std::to_string(character);
        std::cout << err << std::endl;
        throw SemanticException(nullptr);
    }
}

/**
 * Prints the scope tree
 * @param global - scope to print
 */
void Semantic::printTree(const std::shared_ptr<Scope>& global) {
    //Prints heading for output
    std::cout << "Semantic Analysis:" << std::endl;
    std::cout << "------------------------------------------------------------------" << std::endl;
    //Prints the global scope
    printScope(global);
}

/**
 * Prints a given scope, its symbol table, and any child scopes
 * @param scope - to print
 */
void Semantic::printScope(const std::shared_ptr<Scope>& scope) {
    //Stores the tabs to print for indenting
    static std::string tabStr;

    //Identifies the block type to print
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

    //Adds an additional tab for each additional scope
    tabStr += "\t";

    //Prints an open brace
    std:: cout << "{" << std::endl;

    //Prints each symbol in the symbol table
    for (std::pair<std::string, std::pair<Object, Type>> symbol : scope->getSymbolTable()) {
        //Prints the symbol ID and an opening [ indicating the start of a symbol table entry
        std::cout << tabStr << "[\"ID\":" << symbol.first << ", ";

        //Prints whether object is a variable or procedure
        switch (symbol.second.first) {
            case Object::VAR:
                std::cout << "var, ";
                break;
            case Object::PROC:
                std::cout << "proc, ";
                break;
        }

        //Prints the type of the object (int, bool, or string)
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

        //Closes symbol table entry
        std::cout << "]" << std::endl;
    }

    //Prints each of the child scopes
    for (std::shared_ptr<Scope> s : scope->getScopes()) {
        std::cout << std::endl;
        printScope(s);
    }

    //Removes a tab for the number of tabs to print
    if (tabStr.length() > 0) {
        tabStr.erase(tabStr.length() - 1);
    }

    //Prints a closing brace to indicate the end of the scope
    std::cout << tabStr << "}" << std::endl;
}



