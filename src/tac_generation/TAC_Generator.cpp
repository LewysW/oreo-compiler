#include "TAC_Generator.h"
#include "../semantic/Semantic.h"
#include "../parser/Lexer.h"

std::string TAC_Generator::getNextID() {
    return std::string("t" + std::to_string(tempIDNum++));
}

std::string TAC_Generator::getNextLabel() {
    return std::string("L" + std::to_string(labelNum++));
}

void TAC_Generator::generate(const std::shared_ptr<TreeNode> &parseTree) {
    for (const std::shared_ptr<TreeNode> &node : parseTree->getChildren()) {
        //Validate Compound of global scope
        if (node->getLabel() == "Compound") {
            scope(node);
        }
    }
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
        } else if (label == "While" || label == "If") {
            conditionalStmt(node);
        } else if (label == "Assignment") {
            //assignment(node);
        } else if (label == "Function Signature") {
            //functionSig(node);
        } else if (label == "Function Call") {
            //functionCall(node);
        } else if (label == "Return Statement") {
            //returnStmt(node);
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

void TAC_Generator::conditionalStmt(const std::shared_ptr<TreeNode> &parseTree) {
    std::string temp;
    //TODO - change setStartOfBlock to markBlock() and add condition for while vs if using TokenType
    //Identifies the correct part of the conditional statement generate code for
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {
        //Converts the condition expression to instructions
        if (node->getLabel() == "Expression") {
            temp = getNextID();
            addInstruction("ASSIGN", expression(node), std::string(), temp);
            setStartOfBlock(true);
            setBlockLabel(getNextLabel());
            //Converts the statements of the conditional statement to instructions
        } else if (node->getLabel() == "Compound") {
            scope(node);

            //Converts the statements of else to instructions
        } else if (node->getLabel() == "Else") {
            setStartOfBlock(true);
            setBlockLabel(getNextLabel());
            conditionalStmt(node);
        }
    }
}

bool TAC_Generator::isStartOfBlock() const {
    return startOfBlock;
}

void TAC_Generator::setStartOfBlock(bool startOfBlock) {
    TAC_Generator::startOfBlock = startOfBlock;
}

const std::string &TAC_Generator::getBlockLabel() const {
    return blockLabel;
}

void TAC_Generator::setBlockLabel(const std::string &blockLabel) {
    TAC_Generator::blockLabel = blockLabel;
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
    if (isStartOfBlock()) {
        instruction.setLabel(getBlockLabel());
        setStartOfBlock(false);
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

    for (const Instruction& instruction : instructions) {
        std::string instructionStr;

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
}