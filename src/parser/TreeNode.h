#pragma once

#include <vector>
#include "Token.h"

#include <memory>

/**
 * Represents a node in the abstract syntax tree
 */
class TreeNode {
private:
    //Children of node
    std::vector<std::shared_ptr<TreeNode>> children;

    //Optional token of node if not a non-terminal
    Token token;

    //Label of node (e.g. non-terminal or terminal value)
    std::string label;

public:

    //Constructors

    explicit TreeNode(std::string l);

    TreeNode(std::string l, Token t);

    //Getter for children
    const std::vector<std::shared_ptr<TreeNode>> &getChildren() const;

    //Allows addition of child node
    void addChild(std::shared_ptr<TreeNode> child);

    //Allows update to label string
    void setLabel(const std::string &label);

    //Getter for node token
    const Token &getToken() const;

};


