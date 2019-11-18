#include "TypeChecker.h"
#include "TypeException.h"
#include "../parser/Lexer.h"

void TypeChecker::checkTypes(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope>& global) {
    try {
        for (const std::shared_ptr<TreeNode> &node : parseTree->getChildren()) {
            //Validate Compound of global scope
            if (node->getLabel() == "Compound") {
                validateScopeTypes(node, global);
            }
        }
    } catch (TypeException& e) {
        exit(4);
    }
}

void TypeChecker::validateScopeTypes(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope> &scope) {
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getLabel() == "Statement") {
            statement(node, scope);
        }
    }
}

void TypeChecker::statement(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope>& scope) {
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

void TypeChecker::variable(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope>& scope) {
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
            case Pattern::TokenType::ID:
                id = node->getToken().getValue();
                type = scope->getSymbolTable().at(id).second;
                break;
        }
    }

    //If the statement was a variable declaration and assignment
    if (isAssignment) {
        for (const std::shared_ptr<TreeNode>& node : current->getChildren()) {
            //Evaluate the types in the expression of the assignment
            if (node->getLabel() == "Expression") {
                expression(node, scope, type);
            }
        }
    }
}

void TypeChecker::expression(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope> &scope,
                             Type expected) {
    Type result;

    if ((result = evaluateExpression(parseTree, scope)) != expected) {
        generateTypeError(expected, result, parseTree->getToken().getLineNum());
        //TODO - need to validate expression type using evaluateExpression()
        // TODO - function which will use the operators table and will call expression recursively
    }
}

Type TypeChecker::evaluateExpression(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope &> scope) {
    //TODO - if ID is encountered (except in case of not), take as first operator
    //TODO - then check for operator
    //TODO - then check for expression

    return Type::BOOL;
}

void TypeChecker::generateTypeError(Type expected, Type result, unsigned long lineNum) {
    std::string err = "Error: expected expression of type ";

    switch (expected) {
        case Type::INT:
            err += "int";
            break;
        case Type::STRING:
            err += "string";
            break;
        case Type::BOOL:
            err += "bool";
            break;
        default:
            break;
    }

    err += " on line " + std::to_string(lineNum);
    err += ", but got expression of type ";

    switch (result) {
        case Type::INT:
            err += "int";
            break;
        case Type::STRING:
            err += "string";
            break;
        case Type::BOOL:
            err += "bool";
            break;
        default:
            break;
    }

    throw TypeException(nullptr);
}

void TypeChecker::generateOperatorError(Pattern::TokenType op, Type op1, Type op2) {
    std::string err = "Error: operator '" + Lexer::TOKEN_STRINGS[static_cast<unsigned long>(op)] + "'";
    std::vector<Type> opTypes;
    std::vector<std::string> opStrings;
    opTypes.emplace_back(operators.at(op).getOperands().first);
    opTypes.emplace_back(operators.at(op).getOperands().second);
    opTypes.emplace_back(op1);
    opTypes.emplace_back(op2);

    for (Type t : opTypes) {
        switch (t) {
            case Type::INT:
                opStrings.emplace_back("int");
                break;
            case Type::BOOL:
                opStrings.emplace_back("bool");
                break;
            case Type::STRING:
                opStrings.emplace_back("string");
                break;
            case Type::NONE:
                opStrings.emplace_back("none");
        }
    }

    err += " takes ";
    err += (op == Pattern::TokenType::NOT) ? "operand" : "operands";
    err += " of type ";
    err += (op == Pattern::TokenType::NOT) ? opStrings[1] : "<" + opStrings[0] + ", " + opStrings[1] + ">";
    err += "\n";
    err += "Cannot apply to ";
    err += (op == Pattern::TokenType::NOT) ? "operand" : "operands";
    err += " of type ";
    err += (op == Pattern::TokenType::NOT) ? opStrings[3] : "<" + opStrings[2] + ", " + opStrings[3] + ">";

    throw TypeException(nullptr);
}




