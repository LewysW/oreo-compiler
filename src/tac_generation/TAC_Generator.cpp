#include <vector>
#include <map>
#include "TAC_Generator.h"
#include "../semantic/Semantic.h"
#include "../parser/Lexer.h"

std::string TAC_Generator::getNextID() {
    std::string id = ("t" + std::to_string(tempIDNum++));

    //Iterates through strings of instructions
    auto it = find_if(instructions.begin(), instructions.end(), [&id](const Instruction& obj) {return obj.getResult() == id;});

    //Makes sure that temp variable name has not already been declared
    if (it != instructions.end())
        return getNextID();

    return id;
}

std::string TAC_Generator::getNextLabel() {
    return std::string("L" + std::to_string(labelNum++));
}

void TAC_Generator::generate(const std::shared_ptr<TreeNode> &parseTree) {
    addInstruction("BeginProg", std::string(), std::string(), std::string());
    for (const std::shared_ptr<TreeNode> &node : parseTree->getChildren()) {
        //Validate Compound of global scope
        if (node->getLabel() == "Compound") {
            scope(node);
        }
    }
    addInstruction("EndProg", std::string(), std::string(), std::string());
}

void TAC_Generator::scope(const std::shared_ptr<TreeNode> &parseTree) {
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getLabel() == "Statement") {
            statement(node);
        }
    }
}

void TAC_Generator::statement(const std::shared_ptr<TreeNode> &parseTree) {
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        std::string label = node->getLabel();

        //Identifies the appropriate statement type to be validated
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

void TAC_Generator::variable(const std::shared_ptr<TreeNode> &parseTree) {
    std::string id;

    //Iterates through each symbol in the variable statement
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getToken().getType() == Pattern::TokenType::ID) {
            id = node->getToken().getValue();
        } else if (node->getLabel() == "Variable Assignment") {
            for (const std::shared_ptr<TreeNode>& child : node->getChildren()) {
                if (child->getLabel() == "Expression") {
                    addInstruction("ASSIGN", expression(child),  std::string(""), id);
                }
            }
        }
    }
}

void TAC_Generator::printStmt(const std::shared_ptr<TreeNode> &parseTree) {
    Token token = parseTree->getChildren().at(0)->getToken();
    std::string temp;
    std::string temp1;

        switch (token.getType()) {
            case Pattern::TokenType::PRINT:
            case Pattern::TokenType::PRINTLN: {
                for (const std::shared_ptr<TreeNode> &node : parseTree->getChildren()) {
                    if (node->getLabel() == "Expression") {
                        temp = getNextID();
                        addInstruction("ASSIGN", expression(node), std::string(), temp);

                        if (token.getType() == Pattern::TokenType::PRINTLN) {
                            temp1 = getNextID();
                            addInstruction("ASSIGN", std::string(""), "\\n", temp1);
                            addInstruction("PushParam", std::string(), temp1, std::string());
                        }

                        addInstruction("PushParam", std::string(), temp, std::string());
                        addInstruction("Call", std::string(), "_Print", std::string());
                        instructions.back().setBranchInstruction(true);
                        addInstruction("PopParams", std::string(), std::string(), std::string());
                    }
                }
                break;
                case Pattern::TokenType::GET:
                for (const std::shared_ptr<TreeNode> &node : parseTree->getChildren()) {
                    switch (node->getToken().getType()) {
                        case Pattern::TokenType::ID:
                            std::string id = node->getToken().getValue();
                            std::string temp = getNextID();
                            addInstruction("PushParam", std::string(), temp, std::string());
                            addInstruction("Call", std::string(), "_ReadLine", std::string());
                            instructions.back().setBranchInstruction(true);
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

void TAC_Generator::ifStmt(const std::shared_ptr<TreeNode> &parseTree) {
    std::string temp;
    //Identifies the correct part of the conditional statement generate code for
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        //Converts the condition expression to instructions
        if (node->getLabel() == "Expression") {
            temp = getNextID();
            addInstruction("ASSIGN", expression(node), std::string(), temp);

            //Branch instruction operator is comparator, operands are value and operation, result is place to jump to
            addInstruction("IfZ", temp, "Goto", "L" + std::to_string(getLabelNum()));
            instructions.back().setBranchInstruction(true);
        } else if (node->getLabel() == "Compound") {
            scope(node);

            //If in if part of conditional,
            if (parseTree->getLabel() == "If") {
                setLabelRequired(true);
                setBlockLabel(getNextLabel());
            }
            //Converts the statements of else to instructions
        } else if (node->getLabel() == "Else") {
            ifStmt(node);
        }
    }
}

void TAC_Generator::whileLoop(const std::shared_ptr<TreeNode> &parseTree) {
    std::string temp;
    std::string start;
    //Identifies the correct part of the conditional statement generate code for
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        //Converts the condition expression to instructions
        if (node->getLabel() == "Expression") {
            setLabelRequired(true);
            start = getNextLabel();
            setBlockLabel(start);

            temp = getNextID();
            addInstruction("ASSIGN", expression(node), std::string(), temp);

            //Branch instruction operator is comparator, operands are value and operation, result is place to jump to
            addInstruction("IfZ", temp, "Goto", "L" + std::to_string(getLabelNum()));
            instructions.back().setBranchInstruction(true);
        } else if (node->getLabel() == "Compound") {
            scope(node);
            addInstruction("Goto", start, std::string(), std::string());
            instructions.back().setBranchInstruction(true);
            setBlockLabel(getNextLabel());
            setLabelRequired(true);
        }
    }
}

void TAC_Generator::assignment(const std::shared_ptr<TreeNode> &parseTree) {
    std::string id;

    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getToken().getType() == Pattern::TokenType::ID) {
            id = node->getToken().getValue();
        } else if (node->getLabel() == "Expression") {
            addInstruction("ASSIGN", expression(node), std::string(""), id);
        }
    }
}

void TAC_Generator::functionSig(const std::shared_ptr<TreeNode> &parseTree) {
    std::string id;

    //iterate through each symbol in the function declaration
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        //Generate code for body of function
        if (node->getToken().getType() == Pattern::TokenType::ID) {
            id = node->getToken().getValue();

            functions[id] = id;

            //Sets value of next label
            setBlockLabel(id);
            //Next time instruction is added, label will be assigned
            setLabelRequired(true);

            addInstruction("BeginFunc", std::string(), std::string(), std::string());
        } else if (node->getLabel() == "Compound") {
            scope(node);
        }
    }
    addInstruction("EndFunc", std::string(), std::string(), std::string());
}

std::string TAC_Generator::functionCall(const std::shared_ptr<TreeNode> &parseTree) {
    std::string id;
    std::stack<std::string> params;
    //iterates through each symbol in the function call
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getToken().getType() == Pattern::TokenType::ID) {
            //If the current token is the function identifier
            id = node->getToken().getValue();
        } else if (node->getLabel() == "Actual Parameter") {
            for (const std::shared_ptr<TreeNode>& child : node->getChildren()) {
                if (child->getLabel() == "Expression") {
                    params.push(expression(node));
                }
            }
        }
    }

    while (!params.empty()) {
        addInstruction("PushParam", std::string(), params.top(), std::string());
        params.pop();
    }
    std::string result = getNextID();
    addInstruction("Call", std::string(), id, result);
    addInstruction("PopParams", std::string(), std::string(), std::string());
    return result;
}

void TAC_Generator::returnStmt(const std::shared_ptr<TreeNode> &parseTree) {
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        if (node->getLabel() == "Expression") {
            std::string temp = getNextID();
            addInstruction("Return", std::string(), expression(node), std::string());
        }
    }
}

std::string TAC_Generator::expression(const std::shared_ptr<TreeNode>& parseTree) {
    std::string op;
    std::string arg1;
    std::string arg2;
    std::string result;

    //Iterates through each symbol in the expression
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        switch (node->getToken().getType()) {
            //Identifies terminals/ID as arg1
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

                    //Check if bracketed expression within operation
                    for (const std::shared_ptr<TreeNode>& child : node->getChildren()) {
                        if (child->getLabel() == "Expression") {
                            arg2 = expression(child);
                        }
                    }

                    //Otherwise process as normal
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

    return result;
}

bool TAC_Generator::isOperation(const std::string& label) {
    return (Semantic::labelToToken.find(label) != Semantic::labelToToken.end());
}

void TAC_Generator::printInstructions() {
    std::cout << "Three Address Code Generation:" << std::endl;
    std::cout << "------------------------------------------------------------------" << std::endl;

    //Print main program
    for (const Instruction& instruction : instructions) {
            printInstruction(instruction);
    }
}

void TAC_Generator::printInstruction(Instruction instruction) {
    std::string instructionStr;

    if (instruction.isBranchInstruction()) {
        instructionStr += "\t " + instruction.getOp() + " ";
        instructionStr += instruction.getArg1() + " ";
        instructionStr += instruction.getArg2() + " ";
        instructionStr += instruction.getResult() + " ";
        std::cout << instructionStr << ";" << std::endl;
        return;
    }

    if (!instruction.getResult().empty()) {
        instructionStr += instruction.getResult() + " = ";
    }

    if (!instruction.getArg1().empty()) {
        instructionStr += instruction.getArg1() + " ";
    }

    if (instruction.getOp() != "ASSIGN") {
        if (isOperation(instruction.getOp())) {
            instructionStr += Lexer::TOKEN_STRINGS[static_cast<unsigned long>(Semantic::labelToToken.at(instruction.getOp()))];
        } else {
            instructionStr += instruction.getOp() + " ";
        }

    }

    instructionStr += " " + instruction.getArg2() + ";";

    std::string temp = instructionStr;
    instructionStr = (!instruction.getLabel().empty()) ? instruction.getLabel() + ": " : "\t ";
    instructionStr += temp;

    std::cout << " " + instructionStr << std::endl;
}


bool TAC_Generator::isLabelRequired() const {
    return labelRequired;
}

void TAC_Generator::setLabelRequired(bool labelRequired) {
    TAC_Generator::labelRequired = labelRequired;
}

unsigned long TAC_Generator::getLabelNum() const {
    return labelNum;
}

const std::string &TAC_Generator::getBlockLabel() const {
    return blockLabel;
}

void TAC_Generator::setBlockLabel(const std::string &blockLabel) {
    TAC_Generator::blockLabel = blockLabel;
}

const std::vector<Instruction> &TAC_Generator::getInstructions() const {
    return instructions;
}

void TAC_Generator::setInstructions(const std::vector<Instruction> &instructions) {
    TAC_Generator::instructions = instructions;
}

const std::map<std::string, std::string> &TAC_Generator::getFunctions() const {
    return functions;
}
