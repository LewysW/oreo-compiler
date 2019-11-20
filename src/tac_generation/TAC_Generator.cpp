#include "TAC_Generator.h"
#include "../semantic/Semantic.h"
#include "../parser/Lexer.h"

std::string TAC_Generator::getNextID() {
    return std::string("t" + std::to_string(tempIDNum++));
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
            //printStmt(node);
        } else if (label == "While" || label == "If") {
            //conditionalStmt(node);
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
                    Instruction instruction(Pattern::TokenType::ASSIGN, expression(child), "", id);
                    instructions.emplace_back(instruction);
                }
            }
        }
    }
}

std::string TAC_Generator::expression(const std::shared_ptr<TreeNode>& parseTree) {
    Pattern::TokenType op = Pattern::TokenType::NONE;
    std::string arg1;
    std::string arg2;
    std::string result;

    //Iterates through each symbol in the expression
    for (const std::shared_ptr<TreeNode>& node : parseTree->getChildren()) {;
        switch (node->getToken().getType()) {
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
                if (node->getLabel() == "Expression") {
                    arg1 = expression(node);
                } else if (isOperation(node->getLabel())) {
                    op = Semantic::labelToToken.at(node->getLabel());

                    for (const std::shared_ptr<TreeNode>& child : node->getChildren()) {
                        if (child->getLabel() == "Expression") {
                            arg2 = expression(child);
                        } else if (isOperation(child->getLabel())) {
                            arg2 = expression(node);
                        }
                    }
                }
        }
    }

    //Generate new temporary variable ID e.g. t1, t2, t3...
    result = getNextID();
    //Add instruction to queue
    instructions.emplace_back(Instruction(op, arg1, arg2, result));
    return result;
}

bool TAC_Generator::isOperation(const std::string& label) {
    return (Semantic::labelToToken.find(label) != Semantic::labelToToken.end());
}

void TAC_Generator::printInstructions() {
    std::cout << "Three Address Code Generation:" << std::endl;
    std::cout << "------------------------------------------------------------------" << std::endl;
    for (const Instruction& instruction : instructions) {
        std::cout << instruction.getResult() + " = ";
        std::cout << instruction.getArg1() + " ";
        if (instruction.getOp() != Pattern::TokenType::ASSIGN) {
            std::cout << Lexer::TOKEN_STRINGS[static_cast<unsigned long>(instruction.getOp())] + " ";
            std::cout << instruction.getArg2();
        }
        std::cout << ";" << std::endl;
    }
}
