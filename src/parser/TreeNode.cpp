#include "TreeNode.h"

/**
 * Constructor for TreeNode
 * @param l - string label describing token/non-terminal
 */
TreeNode::TreeNode(std::string l)  :
        label(std::move(l)),
        token(Token(Pattern::TokenType::NONE, l, 0, 0))
{
}

/**
 * Constructor for TreeNode
 * @param l - string label describing token/non-terminal
 * @param t - token if terminal
 */
TreeNode::TreeNode(std::string l, Token t)  :
        label(std::move(l)),
        token(std::move(t))
{
}

/**
 * Adds child to back of list of children
 * @param child
 */
void TreeNode::addChild(std::shared_ptr<TreeNode> child) {
    children.emplace_back(child);
}

/**
 * Setter for label
 * @param label - value to assign
 */
void TreeNode::setLabel(const std::string &label) {
    TreeNode::label = label;
}

/**
 * Getter for token
 * @return token
 */
const Token &TreeNode::getToken() const {
    return token;
}

/**
 * Getter for children
 * @return - list of children of node
 */
const std::vector<std::shared_ptr<TreeNode>> &TreeNode::getChildren() const {
    return children;
}
