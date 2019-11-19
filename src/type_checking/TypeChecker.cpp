#include "TypeChecker.h"
#include "TypeException.h"
#include "../parser/Lexer.h"

void TypeChecker::checkTypes(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope>& global) {
    try {
        std::cout << "Type Checking:" << std::endl;
        std::cout << "------------------------------------------------------------------" << std::endl;

        for (const std::shared_ptr<TreeNode> &node : parseTree->getChildren()) {
            //Validate Compound of global scope
            if (node->getLabel() == "Compound") {
                validateScopeTypes(node, global);
            }
        }
        std::cout << "No type errors" << std::endl;
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
        } else if (label == "While" || label == "If") {
            conditionalStmt(node, scope);
        } else if (label == "Assignment") {
            assignment(node, scope);
        } else if (label == "Function Signature") {
            functionSig(node, scope);
        } else if (label == "Function Call") {
            functionCall(node, scope);
        } else if (label == "Return Statement") {
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
    unsigned long line = parseTree->getChildren().front()->getToken().getLineNum();

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
            default:
                break;
        }
    }

    //If the statement was a variable declaration and assignment
    if (isAssignment) {
        for (const std::shared_ptr<TreeNode>& node : current->getChildren()) {
            //Evaluate the types in the expression of the assignment
            if (node->getLabel() == "Expression") {
                expression(node, scope, type, line);
            }
        }
    }
}

void TypeChecker::printStmt(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope> &scope) {
    Token token = parseTree->getChildren().at(0)->getToken();
    Type type;
    unsigned long line = token.getLineNum();

    switch (token.getType()) {
        //If the print statement is print or println, then the inner expressions is validated
        case Pattern::TokenType::PRINT:
        case Pattern::TokenType::PRINTLN:
            for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
                if (node->getLabel() == "Expression") {
                    evaluateExpression(node, scope, line);
                }
            }
            break;
            //If the print statement is get, then the scope of the following ID is validated
        case Pattern::TokenType::GET:
            for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
                if (node->getToken().getType() == Pattern::TokenType::ID) {
                    //Requires that 'get' take a string id
                    if ((type = scope->getSymbol(node->getToken().getValue(), scope).second) != Type::STRING) {
                        std::string err = "Error: 'get' statement on line ";
                        err += std::to_string(node->getToken().getLineNum()) + ", character ";
                        err += std::to_string(node->getToken().getColNum());
                        err += " takes a string identifier as an argument\n" ;
                        err += "Cannot apply to an identifier of type ";

                        switch (type) {
                            case Type::INT:
                                err += "'int'";
                                break;
                            case Type::BOOL:
                                err += "'bool'";
                                break;
                            default:
                                break;
                        }
                        std::cout << err << std::endl;
                        throw TypeException(nullptr);
                    }
                }
            }
        default:
            break;
    }
}

void TypeChecker::conditionalStmt(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope> &scope) {
    unsigned long line = parseTree->getChildren()[0]->getToken().getLineNum();

    //Identifies the correct part of the conditional statement to validate
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getLabel() == "Expression") {
            expression(node, scope, Type::BOOL, line);
        } else if (node->getLabel() == "Compound") {
            validateScopeTypes(node, scope->getScopes().at(scope->getCurrent()));
            scope->setCurrent(scope->getCurrent() + 1);
        } else if (node->getLabel() == "Else") {
            conditionalStmt(node, scope);
        }
    }
}

void TypeChecker::assignment(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope> &scope) {
    Type expected = Type::NONE;
    unsigned long line = 0;

    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getToken().getType() == Pattern::TokenType::ID) {
            expected = scope->getSymbol(node->getToken().getValue(), scope).second;
            line = node->getToken().getLineNum();
        } else if (node->getLabel() == "Expression") {
            //Validates the expression after the := sign
            expression(node, scope, expected, line);
        }
    }
}

void TypeChecker::functionSig(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope> &scope) {
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getLabel() == "Compound") {
            std::shared_ptr<Scope> child = scope->getScopes().at(scope->getCurrent());
            child->setParent(scope);
            validateScopeTypes(node, child);
            scope->setCurrent(scope->getCurrent() + 1);
        }
    }
}

Type TypeChecker::functionCall(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope> &scope) {
    unsigned long line = parseTree->getToken().getLineNum();
    Type retType = Type::NONE;
    std::string funcID;
    std::vector<Type> actualParams;
    std::vector<Type> formalParams;
    std::vector<std::pair<std::string, std::pair<Object, Type>>> functionIDs;

    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        switch (node->getToken().getType()) {
            //Get the return type of the function
            case Pattern::TokenType::ID:
                line = node->getToken().getLineNum();
                funcID = node->getToken().getValue();
                functionIDs = scope->getFuncIDs(funcID, scope);

                for (const std::pair<std::string, std::pair<Object, Type>> entry : functionIDs) {
                    if (entry.second.first == Object::PARAM) {
                        formalParams.emplace_back(entry.second.second);
                    } else {
                        break;
                    }
                }

                retType = scope->getSymbol(node->getToken().getValue(), scope).second;
                break;
            default:
                break;
        }

        //Stores actual parameters
        if (node->getLabel() == "Actual Parameter") {
            for (const std::shared_ptr<TreeNode>& child : node->getChildren()) {
                if (child->getLabel() == "Expression") {
                    actualParams.emplace_back(evaluateExpression(child, scope, child->getToken().getLineNum()));
                }
            }
        }
    }

    //Validate actual parameters against formal parameters:
    //If dimensions of parameters match
    if (formalParams.size() == actualParams.size()) {
        //Compare individual parameters
        for (unsigned long t = 0; t < formalParams.size(); t++) {
            //If they do not match throw an error
            if (formalParams.at(t) != actualParams.at(t)) {
                generateParameterError(funcID, formalParams, actualParams, line);
            }
        }
    //Otherwise if dimensions do not match, throw error
    } else {
        generateParameterError(funcID, formalParams, actualParams, line);
    }

    return retType;
}

void TypeChecker::returnStmt(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope> &scope) {
    Type returnType = scope->getReturnType(scope);
    unsigned long line = parseTree->getChildren()[0]->getToken().getLineNum();

    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        //Validates the types of the expression of a return statement
        if (node->getLabel() == "Expression") {
            expression(node, scope, returnType, line);
            break;
        }
    }
}

void TypeChecker::expression(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope> &scope,
                             Type expected, unsigned long line) {
    Type result;
    if ((result = evaluateExpression(parseTree, scope, line)) != expected) {
        generateTypeError(expected, result, line);
    }
}

/**
 * Evaluates an expression and returns the resulting type
 * if all operators are provided the correct operands
 * @param parseTree - to type check
 * @param scope - to find the type of symbols
 * @param line - to print if an error occurs
 */
Type TypeChecker::evaluateExpression(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope>& scope, unsigned long line) {
    Type op1 = Type::NONE;
    Type op2 = Type::NONE;
    std::string id;
    Operator myOperator;
    Pattern::TokenType type;

    //Iterates through the children of the current expression
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        Pattern::TokenType temp = node->getToken().getType();

        //Identifies the first operand of the current expression if something other than the not expression
        if (temp == Pattern::TokenType::ID) {
            id = node->getToken().getValue();
            op1 = scope->getSymbol(id, scope).second;
        } else if (temp == Pattern::TokenType::TRUE || temp == Pattern::TokenType::FALSE) {
            op1 = Type::BOOL;
        } else if (temp == Pattern::TokenType::STRING_LITERAL) {
            op1 = Type::STRING;
        } else if (temp == Pattern::TokenType::NUM) {
            op1 = Type::INT;
        //Identifies subexpression as second operand in the case of a bracketed expression
        } else if (node->getLabel() == "Function Call") {
            op1 = functionCall(node, scope);
        } else if (node->getLabel() == "Expression") {
            return evaluateExpression(node, scope, line);
        //Identifies a subexpression as second operand in the case of an operator
        } else if (Semantic::labelToToken.find(node->getLabel()) != Semantic::labelToToken.end()) {
            //records type of expression for error handling
            type = Semantic::labelToToken.at(node->getLabel());

            //Gets the operator information associated with the current expression label
            myOperator = operators.at(Semantic::labelToToken.at(node->getLabel()));

            //Evaluates the subexpression of the operator and assigns the result as the second operand
            for (const std::shared_ptr<TreeNode>& child : node->getChildren()) {
                //If another expression with subexpression
                if (child->getLabel() == "Expression") {
                    op2 = evaluateExpression(child, scope, line);
                //Otherwise if a terminal subexpression
                } else if (Semantic::labelToToken.find(node->getLabel()) != Semantic::labelToToken.end()) {
                    op2 = evaluateExpression(node, scope, line);
                }
            }
            //If operands are valid for current expression, store value in op1
            if (myOperator.getOperands().first == op1 && myOperator.getOperands().second == op2) {
                op1 = operators.at(type).getOutput();
                op2 = Type::NONE;
            } else {
                generateOperatorError(type, op1, op2, line);
            }
        }
    }

    //If operands are valid for current expression, store value in op1
    return op1;
}

void TypeChecker::generateTypeError(Type expected, Type result, unsigned long lineNum) {
    std::string err = "Error: expected expression of type ";

    switch (expected) {
        case Type::INT:
            err += "'int'";
            break;
        case Type::STRING:
            err += "'string'";
            break;
        case Type::BOOL:
            err += "'bool'";
            break;
        default:
            break;
    }

    err += " on line " + std::to_string(lineNum);
    err += ", but got expression of type ";

    switch (result) {
        case Type::INT:
            err += "'int'";
            break;
        case Type::STRING:
            err += "'string'";
            break;
        case Type::BOOL:
            err += "'bool'";
            break;
        default:
            break;
    }

    std::cout << err << std::endl;
    throw TypeException(nullptr);
}

void TypeChecker::generateOperatorError(Pattern::TokenType type, Type op1, Type op2, unsigned long lineNum) {
    std::vector<Type> opTypes;
    std::vector<std::string> opStrings;

    opTypes.emplace_back(operators.at(type).getOperands().first);
    opTypes.emplace_back(operators.at(type).getOperands().second);
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
    std::string err = "Error: operator '" + Lexer::TOKEN_STRINGS[static_cast<unsigned long>(type)] + "'";
    err += " on line " + std::to_string(lineNum) + " takes ";
    err += (type == Pattern::TokenType::NOT) ? "operand" : "operands";
    err += " of type ";
    err += (type == Pattern::TokenType::NOT) ? "'" + opStrings[1] + "'" : "<" + opStrings[0] + ", " + opStrings[1] + ">";
    err += "\n";
    err += "Cannot apply to ";
    err += (type == Pattern::TokenType::NOT) ? "operand" : "operands";
    err += " of type ";
    err += (type == Pattern::TokenType::NOT) ? "'" + opStrings[3] + "'" : "<" + opStrings[2] + ", " + opStrings[3] + ">";

    std::cout << err << std::endl;
    throw TypeException(nullptr);
}

void TypeChecker::generateParameterError(const std::string& funcID, const std::vector<Type>& formal, const std::vector<Type>& actual,
                                         unsigned long line) {
    std::string formalStr = "<";
    std::string actualStr = "<";

    for (auto type = formal.begin(); type != formal.end(); type++) {
        switch (*type) {
            case Type::INT:
                formalStr += "int";
                break;
            case Type::BOOL:
                formalStr += "bool";
                break;
            case Type::STRING:
                formalStr += "string";
                break;
            default:
                break;
        }

        if (type + 1 != formal.end()) {
            formalStr += ", ";
        }
    }
    formalStr += ">";

    for (auto type = actual.begin(); type != actual.end(); type++) {
        switch (*type) {
            case Type::INT:
                actualStr += "int";
                break;
            case Type::BOOL:
                actualStr += "bool";
                break;
            case Type::STRING:
                actualStr += "string";
                break;
            default:
                break;
        }

        if (type + 1 != actual.end()) {
            actualStr += ", ";
        }
    }
    actualStr += ">";

    std::string err = "Error: parameter mismatch for call to procedure '" + funcID + "' on line " + std::to_string(line);
    err += "\nProcedure expects arguments of type " + formalStr;
    err += "\nCannot apply to arguments of type " + actualStr;

    std::cout << err << std::endl;
    throw TypeException(nullptr);
}




