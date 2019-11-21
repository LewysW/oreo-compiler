#include <vector>
#include <map>
#include "TAC_Generator.h"
#include "../semantic/Semantic.h"
#include "../parser/Lexer.h"

/**
 * Gets next temporary variable ID to use (e.g. t1, t2, t3...)
 * @return next temp variable ID
 */
std::string TAC_Generator::getNextID() {
    std::string id = ("t" + std::to_string(tempIDNum++));

    //Iterates through strings of instructions
    auto it = find_if(instructions.begin(), instructions.end(), [&id](const Instruction& obj) {return obj.getResult() == id;});

    //Makes sure that temp variable name is not duplicated
    if (it != instructions.end())
        return getNextID();

    return id;
}

/**
 * Gets next label to prepend to instruction
 * @return next label
 */
std::string TAC_Generator::getNextLabel() {
    return std::string("L" + std::to_string(labelNum++));
}

/**
 * Translates given parse tree into vector of
 * three address code instructions and stores in attribute
 * @param parseTree - to translate
 */
void TAC_Generator::generate(const std::shared_ptr<TreeNode> &parseTree) {
    //Adds start of program instruction
    addInstruction("BeginProg", std::string(), std::string(), std::string());
    for (const std::shared_ptr<TreeNode> &node : parseTree->getChildren()) {
        //Validate Compound of global scope
        if (node->getLabel() == "Compound") {
            scope(node);
        }
    }
    //End of program
    addInstruction("EndProg", std::string(), std::string(), std::string());
}

/**
 * Translates the current scope into three address code
 * @param parseTree - to translate
 */
void TAC_Generator::scope(const std::shared_ptr<TreeNode> &parseTree) {
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getLabel() == "Statement") {
            statement(node);
        }
    }
}

/**
 * //Translates the current statement into three address code
 * @param parseTree - to translate
 */
void TAC_Generator::statement(const std::shared_ptr<TreeNode> &parseTree) {
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        std::string label = node->getLabel();

        //Identifies the appropriate statement type to be translated
        //and calls the appropriate function
        if (label == "Variable") {
            variable(node);
        } else if (label == "Print Statement") {
            printStmt(node);
        } else if (label == "If") {
            ifStmt(node);
        } else if (label == "While"){
            whileLoop(node);
        } else if (label == "Assignment") {
            assignment(node);
        } else if (label == "Function Signature") {
            functionSig(node);
        } else if (label == "Function Call") {
            functionCall(node);
        } else if (label == "Return Statement") {
            returnStmt(node);
        }
    }
}

/**
 * Translates the current variable into
 * three address code (if not a simple declaration)
 * @param parseTree - to translate
 */
void TAC_Generator::variable(const std::shared_ptr<TreeNode> &parseTree) {
    std::string id;

    //Iterates through each symbol in the variable statement
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        //Stores ID
        if (node->getToken().getType() == Pattern::TokenType::ID) {
            id = node->getToken().getValue();
        //If assignment
        } else if (node->getLabel() == "Variable Assignment") {
            for (const std::shared_ptr<TreeNode>& child : node->getChildren()) {
                //Add assignment instruction with variable ID as result
                if (child->getLabel() == "Expression") {
                    addInstruction("ASSIGN", expression(child),  std::string(""), id);
                }
            }
        }
    }
}

/**
 * Translates the current print statement into three address code
 * @param parseTree - to translate
 */
void TAC_Generator::printStmt(const std::shared_ptr<TreeNode> &parseTree) {
    Token token = parseTree->getChildren().at(0)->getToken();
    std::string temp;
    std::string temp1;

        switch (token.getType()) {
            //If print or println
            case Pattern::TokenType::PRINT:
            case Pattern::TokenType::PRINTLN: {
                for (const std::shared_ptr<TreeNode> &node : parseTree->getChildren()) {
                    //If expression of print statement
                    if (node->getLabel() == "Expression") {
                        //Store result of expression in temporary ID
                        temp = getNextID();
                        //Add instruction storing expression in temporary ID
                        addInstruction("ASSIGN", expression(node), std::string(), temp);

                        //If println
                        if (token.getType() == Pattern::TokenType::PRINTLN) {
                            temp1 = getNextID();
                            //Add instruction assigning newline to temporary variable
                            addInstruction("ASSIGN", std::string(""), "\\n", temp1);
                            //Push newline parameter when calling library function "_Print"
                            addInstruction("PushParam", std::string(), temp1, std::string());
                        }

                        //Add instruction pushing value to print as parameter
                        addInstruction("PushParam", std::string(), temp, std::string());
                        //Add instruction calling _Print function
                        addInstruction("Call", std::string(), "_Print", std::string());
                        //Indicate branch instruction
                        instructions.back().setBranchInstruction(true);
                        //Add instruciton popping params
                        addInstruction("PopParams", std::string(), std::string(), std::string());
                    }
                }
                break;
                //If print statement is get
                case Pattern::TokenType::GET:
                for (const std::shared_ptr<TreeNode> &node : parseTree->getChildren()) {
                    switch (node->getToken().getType()) {
                        //Record ID
                        case Pattern::TokenType::ID:
                            std::string id = node->getToken().getValue();
                            std::string temp = getNextID();
                            //Add instruction pushing temporary variable onto stack
                            addInstruction("PushParam", std::string(), temp, std::string());
                            //Add instruction calling _Readline function
                            addInstruction("Call", std::string(), "_ReadLine", std::string());
                            //Set as branching instruction
                            instructions.back().setBranchInstruction(true);
                            //Add instruction assigning result of _Readline (stored in temporary ID) to ID in get statement
                            addInstruction("ASSIGN", std::string(), temp, id);
                            break;
                    }
                }
                break;
            }
            default:
                break;
        }
}

/**
 * Translates the current if/else statement into three address code
 * @param parseTree - to translate
 */
void TAC_Generator::ifStmt(const std::shared_ptr<TreeNode> &parseTree) {
    std::string temp;
    //Identifies the correct part of the conditional statement generate code for
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        //Converts the condition expression to instructions
        if (node->getLabel() == "Expression") {
            temp = getNextID();
            //Assigns result of conditional expression to temporary variable
            addInstruction("ASSIGN", expression(node), std::string(), temp);

            //Add instruction to branch to end of block if conditional exprssion is false
            addInstruction("IfZ", temp, "Goto", "L" + std::to_string(getLabelNum()));
            //Mark as branch instruction
            instructions.back().setBranchInstruction(true);
        } else if (node->getLabel() == "Compound") {
            //Translate statements in block of if/else
            scope(node);

            //If in 'if' part of conditional,
            if (parseTree->getLabel() == "If") {
                //Require label to branch to end of if
                setLabelRequired(true);
                setBlockLabel(getNextLabel());
            }
            //Converts the statements of else to instructions
        } else if (node->getLabel() == "Else") {
            //Translate else block to three address code
            ifStmt(node);
        }
    }
}

/**
 * Translates the current while loop into three address code
 * @param parseTree - to translate
 */
void TAC_Generator::whileLoop(const std::shared_ptr<TreeNode> &parseTree) {
    std::string temp;
    std::string start;
    //Identifies the correct part of the conditional statement generate code for
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        //Converts the conditional expression to instructions
        if (node->getLabel() == "Expression") {
            setLabelRequired(true);
            start = getNextLabel();
            setBlockLabel(start);

            temp = getNextID();
            //Adds instruction to store result of conditional expression
            addInstruction("ASSIGN", expression(node), std::string(), temp);

            //Adds instruction to break out of loop if condition is false
            addInstruction("IfZ", temp, "Goto", "L" + std::to_string(getLabelNum()));
            //Marks as branching instruction
            instructions.back().setBranchInstruction(true);
        } else if (node->getLabel() == "Compound") {
            //Translate statements in body of while loop
            scope(node);
            //Adds instruction to goto start of loop
            addInstruction("Goto", start, std::string(), std::string());
            instructions.back().setBranchInstruction(true);
            //Next statement after loop requires matching label from earlier condition
            setBlockLabel(getNextLabel());
            setLabelRequired(true);
        }
    }
}

/**
 * Translates the current assignment into three addrss code
 * @param parseTree
 */
void TAC_Generator::assignment(const std::shared_ptr<TreeNode> &parseTree) {
    std::string id;

    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getToken().getType() == Pattern::TokenType::ID) {
            //Stores the ID of the variable being assigned
            id = node->getToken().getValue();
        } else if (node->getLabel() == "Expression") {
            //Adds an instruction which stores the result of the assignment expression
            addInstruction("ASSIGN", expression(node), std::string(""), id);
        }
    }
}

/**
 * Translates the current function signature/declaration into three address code
 * @param parseTree - to translate
 */
void TAC_Generator::functionSig(const std::shared_ptr<TreeNode> &parseTree) {
    std::string id;

    //iterate through each symbol in the function declaration
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        //Generate code for body of function
        if (node->getToken().getType() == Pattern::TokenType::ID) {
            id = node->getToken().getValue();

            //Sets value of next label
            setBlockLabel(id);
            //Next time instruction is added, label will be assigned
            setLabelRequired(true);

            //Add instruction indicating beginning of function
            addInstruction("BeginFunc", std::string(), std::string(), std::string());
        //Translate instructions in function body
        } else if (node->getLabel() == "Compound") {
            scope(node);
        }
    }
    //Add instruction indicating end of function
    addInstruction("EndFunc", std::string(), std::string(), std::string());
}

/**
 * Translates the current function call into three address code
 * @param parseTree - to translate
 * @return temporary ID storing result of function call (if required by an expression)
 */
std::string TAC_Generator::functionCall(const std::shared_ptr<TreeNode> &parseTree) {
    std::string id;
    std::stack<std::string> params;

    //iterates through each symbol in the function call
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getToken().getType() == Pattern::TokenType::ID) {
            //If the current token is the function identifier, store it
            id = node->getToken().getValue();
        //If the current node is an actual parameter
        } else if (node->getLabel() == "Actual Parameter") {
            for (const std::shared_ptr<TreeNode>& child : node->getChildren()) {
                if (child->getLabel() == "Expression") {
                    //Evaluate the parameter expression and push onto stack of parameters
                    params.push(expression(node));
                }
            }
        }
    }

    //Until no more parameters
    while (!params.empty()) {
        //Push params in reverse order
        addInstruction("PushParam", std::string(), params.top(), std::string());
        //Remove parameter from stack
        params.pop();
    }

    std::string result = getNextID();
    //Add instruction calling function, storing in temporary result variable
    addInstruction("Call", std::string(), id, result);
    //Add instruction popping parameters
    addInstruction("PopParams", std::string(), std::string(), std::string());
    //Return temporary variable storing result of call
    return result;
}

/**
 * Translates the current return statement into three address code
 * @param parseTree
 */
void TAC_Generator::returnStmt(const std::shared_ptr<TreeNode> &parseTree) {
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getLabel() == "Expression") {
            std::string temp = getNextID();
            addInstruction("Return", std::string(), expression(node), std::string());
        }
    }
}

/**
 * Recursively Translates the current expression into three address code
 * @param parseTree - to translate
 * @return temporary variable storing result of expression or literal value
 */
std::string TAC_Generator::expression(const std::shared_ptr<TreeNode>& parseTree) {
    //Instruction quadruple
    std::string op;
    std::string arg1;
    std::string arg2;
    std::string result;

    //Iterates through each symbol in the expression
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        switch (node->getToken().getType()) {
            //Identifies terminals/ID/function call as arg1
            case Pattern::TokenType::ID:
            case Pattern::TokenType::NUM:
            case Pattern::TokenType::STRING_LITERAL:
                arg1 = node->getToken().getValue();
                break;
            case Pattern::TokenType::TRUE:
                arg1 = "true";
                break;
            case Pattern::TokenType::FALSE:
                arg1 = "false";
                break;
            default:
                if (node->getLabel() == "Function Call") {
                    arg1 = functionCall(node);
                    break;
                }

                //Records intermediary operation
                if (!arg1.empty() && !arg2.empty()) {
                    arg1 = addInstruction(op, arg1, arg2, result);
                    arg2 = "";
                }

                //If bracketed expression, set result of inner expression as first argument
                if (node->getLabel() == "Expression") {
                    arg1 = expression(node);

                //Otherwise if an operation
                } else if (isOperation(node->getLabel())) {
                    //Get operator token
                    op = node->getLabel();

                    //Check if bracketed expression within operation and set as second parameter
                    for (const std::shared_ptr<TreeNode>& child : node->getChildren()) {
                        if (child->getLabel() == "Expression") {
                            arg2 = expression(child);
                        }
                    }

                    //Otherwise if expression is of equal precedence (not in brackets)
                    //process as normal
                    if (arg2.empty()) {
                        arg2 = expression(node);
                    }
                }
        }
    }

    //If terminal symbol, return first argument as result
    if (arg2.empty()) {
        return arg1;
    } else {
        //Otherwise return result of new instruction
        return addInstruction(op, arg1, arg2, result);
    }
}

/**
 * Adds an instruction to the list of instructions
 * @param op - operation of instruction
 * @param arg1 - first argument of instruction
 * @param arg2 - second argument of instruction
 * @param result - result of instruction
 * @return resulting ID of instruction
 */
std::string TAC_Generator::addInstruction(std::string op, std::string arg1, std::string arg2, std::string result) {
    if (result.empty() && isOperation(op)) {
        //Generate new temporary variable ID e.g. t1, t2, t3...
        result = getNextID();
    }

    //Create new instruction
    Instruction instruction(op, arg1, arg2, result);

    //Set label if block
    if (isLabelRequired()) {
        instruction.setLabel(getBlockLabel());
        setLabelRequired(false);
    }

    //Add instruction to queue
    instructions.emplace_back(instruction);

    //Return result ID
    return result;
}

/**
 * returns whether a given parse tree label is an operation (e.g. relational or arithmetic)
 * @param label - to check
 * @return whether label is operation
 */
bool TAC_Generator::isOperation(const std::string& label) {
    return (Semantic::labelToToken.find(label) != Semantic::labelToToken.end());
}

/**
 * Prints the list of instructions
 */
void TAC_Generator::printInstructions() {
    std::cout << "Three Address Code Generation:" << std::endl;
    std::cout << "------------------------------------------------------------------" << std::endl;

    //Print each three address code instruction in program
    for (const Instruction& instruction : instructions) {
            printInstruction(instruction);
    }
}

/**
 * Prints given instruction
 * @param instruction - to print
 */
void TAC_Generator::printInstruction(Instruction instruction) {
    std::string instructionStr;

    //If conditional instruction, simply print the values not as an assignment
    if (instruction.isBranchInstruction()) {
        instructionStr += "\t " + instruction.getOp() + " ";
        instructionStr += instruction.getArg1() + " ";
        instructionStr += instruction.getArg2() + " ";
        instructionStr += instruction.getResult() + " ";
        std::cout << instructionStr << ";" << std::endl;
        return;
    }

    //Otherwise generate string to print:

    //If instruction has a result
    if (!instruction.getResult().empty()) {
        //Add equals sign
        instructionStr += instruction.getResult() + " = ";
    }

    //Add first argument if one is present
    if (!instruction.getArg1().empty()) {
        instructionStr += instruction.getArg1() + " ";
    }

    //If not assignment
    if (instruction.getOp() != "ASSIGN") {
        //If operation of instruction is arithmetic/logical/relational
        if (isOperation(instruction.getOp())) {
            //Get symbol using operation token
            instructionStr += Lexer::TOKEN_STRINGS[static_cast<unsigned long>(Semantic::labelToToken.at(instruction.getOp()))];
        } else {
            //Otherwise simply append the operation string (e.g. being, end, PushParam etc)
            instructionStr += instruction.getOp() + " ";
        }

    }

    //Append second argument (if only one argument is present second one is)
    instructionStr += " " + instruction.getArg2() + ";";

    //Print label prepending instruction if not empty
    std::string temp = instructionStr;
    instructionStr = (!instruction.getLabel().empty()) ? instruction.getLabel() + ": " : "\t ";
    instructionStr += temp;

    //Print instruction string
    std::cout << " " + instructionStr << std::endl;
}

/**
 * Returns whether current instruction needs a label prepended
 * @return labelRequired
 */
bool TAC_Generator::isLabelRequired() const {
    return labelRequired;
}

/**
 * Setter for labelRequired
 * @param labelRequired
 */
void TAC_Generator::setLabelRequired(bool labelRequired) {
    TAC_Generator::labelRequired = labelRequired;
}

/**
 * Getter for number of labels in instructions
 * @return number of next label
 */
unsigned long TAC_Generator::getLabelNum() const {
    return labelNum;
}

/**
 * Getter for blockLabel
 * @return value of label to be prepended
 */
const std::string &TAC_Generator::getBlockLabel() const {
    return blockLabel;
}

/**
 * Setter for blockLabel
 * @param blockLabel - to assign
 */
void TAC_Generator::setBlockLabel(const std::string &blockLabel) {
    TAC_Generator::blockLabel = blockLabel;
}

/**
 * Getter for list of instructions
 * @return instructions - list of instructions
 */
const std::vector<Instruction> &TAC_Generator::getInstructions() const {
    return instructions;
}
