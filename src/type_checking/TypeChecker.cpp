#include "TypeChecker.h"
#include "TypeException.h"
#include "../parser/Lexer.h"

/**
 * Validate the types of a given parse tree
 * @param parseTree - to check
 * @param global - scope to lookup symbols in
 */
void TypeChecker::checkTypes(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope>& global) {
    try {
        std::cout << "Type Checking:" << std::endl;
        std::cout << "------------------------------------------------------------------" << std::endl;

        //Iterates through each scope to check
        for (const std::shared_ptr<TreeNode> &node : parseTree->getChildren()) {
            //Validate Compound of global scope
            if (node->getLabel() == "Compound") {
                validateScopeTypes(node, global);
            }
        }

        //If execution gets here, no type errors have occurred
        std::cout << "No type errors" << std::endl << std::endl;
    //Exception thrown if error occurs
    } catch (TypeException& e) {
        exit(4);
    }
}

/**
 * Validates types of current scope
 * @param parseTree - to check
 * @param scope - to lookup symbols in
 */
void TypeChecker::validateScopeTypes(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope> &scope) {
    //iterate through each statement in scope and checks its types
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getLabel() == "Statement") {
            statement(node, scope);
        }
    }
}

/**
 * Validates the types of the current statement
 * @param parseTree - to check
 * @param scope - to lookup symbols in
 */
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

/**
 * Validates the types of the current variable
 * @param parseTree - to check
 * @param scope - to lookup symbols in
 */
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
            //Records if variable is assignment
            isAssignment = true;
            //Records assignment
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

/**
 * Validates the types of print statements
 * @param parseTree - to check
 * @param scope - to lookup symbols in
 */
void TypeChecker::printStmt(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope> &scope) {
    //Gets print token
    Token token = parseTree->getChildren().at(0)->getToken();
    Type type;
    //Gets line number of token
    unsigned long line = token.getLineNum();

    //Decides between each of the three print statements
    switch (token.getType()) {
        //If the print statement is print or println, then the inner expressions is validated
        case Pattern::TokenType::PRINT:
        case Pattern::TokenType::PRINTLN:
            //Validate types of expressions in print statements
            for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
                if (node->getLabel() == "Expression") {
                    evaluateExpression(node, scope, line);
                }
            }
            break;
            //If the print statement is get, then the type of the following ID is validated
        case Pattern::TokenType::GET:
            for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
                if (node->getToken().getType() == Pattern::TokenType::ID) {
                    //Ensures that get takes a string argument
                    if ((type = scope->getSymbol(node->getToken().getValue(), scope).second) != Type::STRING) {

                        //Throws an error if get is given a bool or int
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

/**
 * Validates conditional statement types and their expressions
 * @param parseTree - to check
 * @param scope - to lookup symbols in
 */
void TypeChecker::conditionalStmt(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope> &scope) {
    unsigned long line = parseTree->getChildren()[0]->getToken().getLineNum();

    //Identifies the correct part of the conditional statement to validate
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        //Ensures that statement condition returns a bool type
        if (node->getLabel() == "Expression") {
            expression(node, scope, Type::BOOL, line);

        //Validates body of conditional statement
        } else if (node->getLabel() == "Compound") {
            validateScopeTypes(node, scope->getScopes().at(scope->getCurrent()));
            scope->setCurrent(scope->getCurrent() + 1);

        //Validates else of an if
        } else if (node->getLabel() == "Else") {
            conditionalStmt(node, scope);
        }
    }
}

/**
 * Validates an assignment statement
 * @param parseTree - to check
 * @param scope - to lookup symbols in
 */
void TypeChecker::assignment(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope> &scope) {
    Type expected = Type::NONE;
    unsigned long line = 0;

    //iterate through each symbol of assignment
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        //If symbol is ID
        if (node->getToken().getType() == Pattern::TokenType::ID) {
            //Record type of variable being assigned
            expected = scope->getSymbol(node->getToken().getValue(), scope).second;
            //Record line number of assignment
            line = node->getToken().getLineNum();
        } else if (node->getLabel() == "Expression") {
            //Validates the types expression after the := sign
            expression(node, scope, expected, line);
        }
    }
}

/**
 * Validates a function signature
 * @param parseTree - to check
 * @param scope - to lookup symbols in
 */
void TypeChecker::functionSig(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope> &scope) {
    //iterate through each symbol in the function declaration
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        //Validate body of function
        if (node->getLabel() == "Compound") {
            //Records current scope as parent
            std::shared_ptr<Scope> child = scope->getScopes().at(scope->getCurrent());
            child->setParent(scope);
            //Validate function scope
            validateScopeTypes(node, child);
            scope->setCurrent(scope->getCurrent() + 1);
        }
    }
}

/**
 * Validates a function call statement and
 * returns its type for use in expression evaluations
 * @param parseTree - to check
 * @param scope - to lookup symbols in
 * @return return type of function called
 */
Type TypeChecker::functionCall(const std::shared_ptr<TreeNode> &parseTree, const std::shared_ptr<Scope> &scope) {
    unsigned long line = parseTree->getToken().getLineNum();
    //Stores return type
    Type retType = Type::NONE;
    //Stores id of function
    std::string funcID;
    //Store actual and formal parameters
    std::vector<Type> actualParams;
    std::vector<Type> formalParams;

    //Stores identifiers of function called
    std::vector<std::pair<std::string, std::pair<Object, Type>>> functionIDs;

    //iterates through each symbol in the function call
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        switch (node->getToken().getType()) {
            //If the current token is the function identifier
            case Pattern::TokenType::ID:
                //Records line number of function call
                line = node->getToken().getLineNum();
                //Records name of function
                funcID = node->getToken().getValue();
                //Gets variables in scope of function declaration
                functionIDs = scope->getFuncIDs(funcID, scope);

                //Records formal parameter types
                for (const std::pair<std::string, std::pair<Object, Type>> entry : functionIDs) {
                    if (entry.second.first == Object::PARAM) {
                        formalParams.emplace_back(entry.second.second);
                    } else {
                        break;
                    }
                }

                //Gets return type of function
                retType = scope->getSymbol(node->getToken().getValue(), scope).second;
                break;
            default:
                break;
        }

        //Stores actual parameters
        if (node->getLabel() == "Actual Parameter") {
            //Validates expression of actual parameters
            for (const std::shared_ptr<TreeNode>& child : node->getChildren()) {
                if (child->getLabel() == "Expression") {
                    //Records actual parameter type returned by expression evaluation
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
            //If parameters do not match
            if (formalParams.at(t) != actualParams.at(t)) {
                //throw parameter error
                generateParameterError(funcID, formalParams, actualParams, line);
            }
        }
    //Otherwise if dimensions do not match, throw error
    } else {
        generateParameterError(funcID, formalParams, actualParams, line);
    }

    //If function call valid, return return type of function
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

        //Identifies the first operand of the current expression if something other than the 'not' expression
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
            //Expression directly contains expression, return subexpression evaluation as result
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
            //Otherwise throw operator error
            } else {
                generateOperatorError(type, op1, op2, line);
            }
        }
    }

    //Returns result of current operation, stored in first operand
    return op1;
}

/**
 * Generate a type error indicating that an expression produced an unexpected type
 * @param expected - expected type of expression
 * @param result - actual type of expression
 * @param lineNum - line of expression
 */
void TypeChecker::generateTypeError(Type expected, Type result, unsigned long lineNum) {
    std::string err = "Error: expected expression of type ";

    //Record expected expression type
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

    //Line of expression
    err += " on line " + std::to_string(lineNum);
    err += ", but got expression of type ";

    //Record result of expression
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

    //Print error message and throw error
    std::cout << err << std::endl;
    throw TypeException(nullptr);
}

/**
 * Generate an operator error,
 * indicating that an operator has been provided with incorrect operands
 * @param type - of operator
 * @param op1 - first operand
 * @param op2 - second operand
 * @param lineNum - line of operator
 */
void TypeChecker::generateOperatorError(Pattern::TokenType type, Type op1, Type op2, unsigned long lineNum) {
    std::vector<Type> opTypes;
    std::vector<std::string> opStrings;

    //Add expected and actual operands to list
    opTypes.emplace_back(operators.at(type).getOperands().first);
    opTypes.emplace_back(operators.at(type).getOperands().second);
    opTypes.emplace_back(op1);
    opTypes.emplace_back(op2);

    //for each operand type in list, construct string of types
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

    //Construct error message
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

    //Print and throw error
    std::cout << err << std::endl;
    throw TypeException(nullptr);
}

/**
 * Generate a parameter error,
 * indicating that a function call has been given an invalid number/type of actual parameters
 * @param funcID - name of function
 * @param formal - formal parameter types
 * @param actual - actual parameter types
 * @param line - line number of function call
 */
void TypeChecker::generateParameterError(const std::string& funcID, const std::vector<Type>& formal, const std::vector<Type>& actual,
                                         unsigned long line) {
    std::string formalStr = "<";
    std::string actualStr = "<";

    //Constructs string of formal parameter types
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

    //Constructs string of actual parameter types
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

    //Constructs error message, showing formal and actual parameter mismatch
    std::string err = "Error: parameter mismatch for call to procedure '" + funcID + "' on line " + std::to_string(line);
    err += "\nProcedure expects arguments of type " + formalStr;
    err += "\nCannot apply to arguments of type " + actualStr;

    //Print error and throw error
    std::cout << err << std::endl;
    throw TypeException(nullptr);
}




