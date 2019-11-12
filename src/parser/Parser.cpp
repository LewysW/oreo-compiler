#include "Parser.h"
using Pattern::TokenType;

/**
 * Constructor for parser
 * Initialises tokens with a sanitised list of tokens and
 * creates a new tree node acting as the root of the parse tree
 * @param toks - list of tokens from lexical analysis
 */
Parser::Parser(const std::vector<Token>& toks) :
        tokens(removeComments(toks)),
        parseTree(std::make_shared<TreeNode>(TreeNode("Program")))
{
    parse();
}

/**
 * Prints the abstract syntax tree
 */
void Parser::printTree() {
    std::cout << "Syntactic Analysis:" << std::endl;
    std::cout << "------------------------------------------------------------------" << std::endl;
    printNode(parseTree);
}

/**
 * Prints a node in the tree
 * @param node - node to be printed
 */
void Parser::printNode(std::shared_ptr<TreeNode> node) {
    //Gets token from node
    Token const& token = node->getToken();
    //Stores number of tabs and commas to print
    static std::string tabStr;
    static std::string commaStr;

    //If token is a terminal
    if (token.getType() != TokenType::NONE) {
        //Surround token in quotes
        //Add value at end of token string if an ID, number, or string literal
        //Print token
        std::string tokStr = "\"" + Lexer::TOKEN_STRINGS.at(static_cast<unsigned long>(token.getType())) + "\"";
        tokStr += ((token.getValue().empty()) ? "" : (": \"" + token.getValue() + "\"")) ;
        std::cout << tabStr << tokStr << commaStr << std::endl;

        //Otherwise if a non-terminal
    } else {
        //Print label of non-terminal follow by ':' and open brace
        std::cout << tabStr << token.getValue() << ":" << " {" << std::endl;

        //Increment number of tabs
        tabStr += "\t";

        //Print each of the children in the current node
        for (auto& t : node->getChildren()) {
            //Print a comma if not the final child
            if (t == node->getChildren().back()) {
                commaStr = "";
            } else {
                commaStr = ",";
            }

            printNode(t);
        }

        //Decrement the number of tabs if leaving a non-terminal
        if (tabStr.length() > 0) {
            tabStr.erase(tabStr.length() - 1);
        }

        //Print a closing brace
        std::cout << tabStr << "}" << std::endl << std::endl;
    }
}

/**
 * Return a list of tokens with the comments removed
 * @param tokens - list of tokens to sanitise
 * @return sanitised list of tokens
 */
std::vector<Token> Parser::removeComments(std::vector<Token> tokens) {
    std::vector<Token> toks;

    for (Token t : tokens) {
        if (t.getType() != Pattern::TokenType::COMMENT)
            toks.emplace_back(t);
    }

    return toks;
}

/**
 * Matches a terminal symbol
 * @param t - expected token
 * @param node - leaf node
 */
void Parser::match(TokenType t, std::shared_ptr<TreeNode> node) {
    //Get next token
    Token current = tokens.front();
    //Get type as string
    std::string type = Lexer::TOKEN_STRINGS.at(static_cast<unsigned long>(current.getType()));

    //If token matches expected token
    if (current.getType() == t) {
        //Add token as leaf node
        TreeNode child(type, current);
        node->addChild(std::make_shared<TreeNode>(child));

        //Remove token from token list
        tokens.erase(tokens.begin());

    //Otherwise throw an error
    } else {
        std::string err = "Error: Invalid token \'" + type;
        err += "\' on line " + std::to_string(current.getLineNum());
        err += ", character " + std::to_string(current.getColNum());
        err += ". Expected token \'" + Lexer::TOKEN_STRINGS[static_cast<unsigned long>(t)];
        err += "\'";
        std::cout << err << std::endl;
        throw ParseException(nullptr);
    }
}

/**
 * Call start symbol function of grammar - prog()
 */
void Parser::parse() {
    try {
        prog(parseTree);
    } catch (ParseException& e) {
        exit(2);
    }
}

/**
 * Represents non-terminal 'P' in the grammar
 * @param node - to add symbols to
 */
void Parser::prog(std::shared_ptr<TreeNode> node) {
    node->setLabel("Program");

    match(TokenType::PROGRAM, node);
    match(TokenType::ID, node);
    compound(node);
}

/**
 * Represents non-terminal 'Compound' in the grammar
 * @param node - to add symbols to
 */
void Parser::compound(std::shared_ptr<TreeNode> node) {
    std::shared_ptr<TreeNode> child = std::make_shared<TreeNode>(TreeNode("Compound"));
    node->addChild(child);

    match(TokenType::BEGIN, child);
    stmt(child);
    stmts(child);
    match(TokenType::END, child);
}

/**
 * Represents non-terminal 'Stmts' in the grammar
 * @param node - to add symbols to
 */
void Parser::stmts(std::shared_ptr<TreeNode> node) {
    TokenType t = tokens.front().getType();

    switch(t) {
        case TokenType::VAR:
        case TokenType::PRINT:
        case TokenType::PRINTLN:
        case TokenType::GET:
        case TokenType::WHILE:
        case TokenType::IF:
        case TokenType::ID:
        case TokenType::PROCEDURE:
            stmt(node);
            stmts(node);
            break;
        default:
            break;
    }
}

/**
 * Represents non-terminal 'Stmt' in the grammar
 * @param node - to add symbols to
 */
void Parser::stmt(std::shared_ptr<TreeNode> node) {
    std::shared_ptr<TreeNode> child = std::make_shared<TreeNode>(TreeNode("Statement"));
    node->addChild(child);

    TokenType t = tokens.front().getType();

    switch (t) {
        case TokenType::VAR:
            variable(child);
            break;
        case TokenType::PRINT:
        case TokenType::PRINTLN:
        case TokenType::GET:
            printStmt(child);
            break;
        case TokenType::WHILE:
            whileLoop(child);
            break;
        case TokenType::IF:
            ifStmt(child);
            break;
        case TokenType::ID:
            operation(child);
            break;
        case TokenType::PROCEDURE:
            funcSig(child);
            break;
        case TokenType::RETURN:
            returnStmt(child);
            break;
        default:
            std::string err = "Error: Invalid statement on line " + tokens.front().getLineNum();
            std::cout << err << std::endl;
            throw ParseException(nullptr);
    }

}

/**
 * Represents non-terminal V in the grammar
 * @param node - to add symbols to
 */
void Parser::variable(std::shared_ptr<TreeNode> node) {
    std::shared_ptr<TreeNode> child = std::make_shared<TreeNode>(TreeNode("Variable"));
    node->addChild(child);

    match(TokenType::VAR, child);
    match(TokenType::ID, child);

    if (tokens.front().getType() == TokenType::ASSIGN) {
        variableAssign(child);
    }

    match(TokenType::SEMI, child);
}

/**
 * Represents non-terminal V' in the grammar
 * @param node - to add symbols to
 */
void Parser::variableAssign(std::shared_ptr<TreeNode> node) {
    std::shared_ptr<TreeNode> child = std::make_shared<TreeNode>(TreeNode("Variable Assignment"));
    node->addChild(child);

    match(TokenType::ASSIGN, child);
    expr(child);
}

/**
 * Represents non-terminal 'Pr' in the grammar
 * @param node - to add symbols to
 */
void Parser::printStmt(std::shared_ptr<TreeNode> node) {
    std::shared_ptr<TreeNode> child = std::make_shared<TreeNode>(TreeNode("Print Statement"));
    node->addChild(child);

    TokenType t = tokens.front().getType();

    switch(t) {
        case TokenType::PRINT:
            match(TokenType::PRINT, child);
            expr(child);
            match(TokenType::SEMI, child);
            break;
        case TokenType::PRINTLN:
            match(TokenType::PRINTLN, child);
            expr(child);
            match(TokenType::SEMI, child);
            break;
        case TokenType::GET:
            match(TokenType::GET, child);
            match(TokenType::ID, child);
            match(TokenType::SEMI, child);
            break;
        default:
            std::string err = "Error: Invalid IO statement on line " + tokens.front().getLineNum();
            std::cout << err << std::endl;
            throw ParseException(nullptr);
    }
}

/**
 * Represents non-terminal 'W' in the grammar
 * @param node - to add symbols to
 */
void Parser::whileLoop(std::shared_ptr<TreeNode> node) {
    std::shared_ptr<TreeNode> child = std::make_shared<TreeNode>(TreeNode("While"));
    node->addChild(child);

    match(TokenType::WHILE, child);
    match(TokenType::LPAREN, child);
    expr(child);
    match(TokenType::RPAREN, child);
    compound(child);
    match(TokenType::SEMI, child);
}

/**
 * Represents non-terminal I in the grammar
 * @param node - to add symbols to
 */
void Parser::ifStmt(std::shared_ptr<TreeNode> node) {
    std::shared_ptr<TreeNode> child = std::make_shared<TreeNode>(TreeNode("If"));
    node->addChild(child);

    match(TokenType::IF, child);
    match(TokenType::LPAREN, child);
    expr(child);
    match(TokenType::RPAREN, child);
    match(TokenType::THEN, child);
    compound(child);

    if (tokens.front().getType() == TokenType::ELSE) {
        elseStmt(child);
    }

    match(TokenType::SEMI, child);
}

/**
 * Represents non-terminal I' in the grammar
 * @param node - to add symbols to
 */
void Parser::elseStmt(std::shared_ptr<TreeNode> node) {
    std::shared_ptr<TreeNode> child = std::make_shared<TreeNode>(TreeNode("Else"));
    node->addChild(child);

    match(TokenType::ELSE, child);
    compound(child);
}

/**
 * Part of implementation of non-terminals A and A' in the grammar
 * @param node - to add symbols to
 */
void Parser::operation(std::shared_ptr<TreeNode> node) {
    std::shared_ptr<TreeNode> temp = std::make_shared<TreeNode>(TreeNode("Operation"));
    match(TokenType::ID, temp);

    std::shared_ptr<TreeNode> child;
    if (tokens.front().getType() == TokenType::ASSIGN) {
        child = std::make_shared<TreeNode>(TreeNode("Assignment"));
        child->addChild(temp->getChildren().front());
        node->addChild(child);
        assign(child);
    } else if (tokens.front().getType() == TokenType::LPAREN) {
        child = std::make_shared<TreeNode>(TreeNode("Function Call"));
        child->addChild(temp->getChildren().front());
        node->addChild(child);
        funcCall(child);
        match(TokenType::SEMI, child);
    }

}

/**
 * Part of implementation of non-terminals A and A' in the grammar
 * @param node - to add symbols to
 */
void Parser::assign(std::shared_ptr<TreeNode> node) {
    match(TokenType::ASSIGN, node);
    expr(node);
    match(TokenType::SEMI, node);
}

/**
 * Represents non-terminal FuncCall in the grammar
 * @param node - to add symbols to
 */
void Parser::funcCall(std::shared_ptr<TreeNode> node) {
    match(TokenType::LPAREN, node);
    switch(tokens.front().getType()) {
        case TokenType::NOT:
        case TokenType::ID:
        case TokenType::LPAREN:
        case TokenType::STRING:
        case TokenType::NUM:
        case TokenType::TRUE:
        case TokenType::FALSE:
            actualParams(node);
            break;
        default:
            break;
    }

    match(TokenType::RPAREN, node);
}

/**
 * Represents non-terminal ActualParams in the grammar
 * @param node - to add symbols to
 */
void Parser::actualParams(std::shared_ptr<TreeNode> node) {
    switch(tokens.front().getType()) {
        case TokenType::NOT:
        case TokenType::ID:
        case TokenType::LPAREN:
        case TokenType::STRING:
        case TokenType::NUM:
        case TokenType::TRUE:
        case TokenType::FALSE: {
            std::shared_ptr<TreeNode> child = std::make_shared<TreeNode>(TreeNode("Actual Parameter"));
            node->addChild(child);

            expr(child);
            actualParam(node);
        }
            break;
        default:
            break;
    }
}

/**
 * Represents non-terminal ActualParam in the grammar
 * @param node - to add symbols to
 */
void Parser::actualParam(std::shared_ptr<TreeNode> node) {
    if (tokens.front().getType() == TokenType::COMMA) {
        match(TokenType::COMMA, node);

        std::shared_ptr<TreeNode> child = std::make_shared<TreeNode>(TreeNode("Actual Parameter"));
        node->addChild(child);

        expr(child);

        actualParam(node);
    }
}

/**
 * Represents non-terminal FuncSig in the grammar
 * @param node - to add symbols to
 */
void Parser::funcSig(std::shared_ptr<TreeNode> node) {
    std::shared_ptr<TreeNode> child = std::make_shared<TreeNode>(TreeNode("Function Signature"));
    node->addChild(child);

    match(TokenType::PROCEDURE, child);
    match(TokenType::ID, child);
    match(TokenType::LPAREN, child);

    if (tokens.front().getType() == TokenType::VAR) {
        formalParams(child);
    }

    match(TokenType::RPAREN, child);
    compound(child);
}

/**
 * Represents non-terminal FormalParams in the grammar
 * @param node - to add symbols to
 */
void Parser::formalParams(std::shared_ptr<TreeNode> node) {
    if (tokens.front().getType() == TokenType::VAR) {
        std::shared_ptr<TreeNode> child = std::make_shared<TreeNode>(TreeNode("Formal Parameter"));
        node->addChild(child);

        match(TokenType::VAR, child);
        match(TokenType::ID, child);
    }

    if (tokens.front().getType() == TokenType::COMMA) {
        formalParam(node);
    }
}

/**
 * Represents non-terminal FormalParam in the grammar
 * @param node - to add symbols to
 */
void Parser::formalParam(std::shared_ptr<TreeNode> node) {
    std::shared_ptr<TreeNode> child = std::make_shared<TreeNode>(TreeNode("Formal Parameter"));
    node->addChild(child);

    match(TokenType::COMMA, child);
    match(TokenType::VAR, child);
    match(TokenType::ID, child);

    if (tokens.front().getType() == TokenType::COMMA) {
        formalParam(node);
    }
}

/**
 * Represents non-terminal Return in the grammar
 * @param node - to add symbols to
 */
void Parser::returnStmt(std::shared_ptr<TreeNode> node) {
    std::shared_ptr<TreeNode> child = std::make_shared<TreeNode>(TreeNode("Return Statement"));
    node->addChild(child);

    match(TokenType::RETURN, child);
    expr(child);
    match(TokenType::SEMI, child);
}

/**
 * Added for readability in parse tree/other functions
 * Nicer to read 'expr' in above non-terminals than orExpr1 everywhere
 * @param node - to add symbols to
 */
void Parser::expr(std::shared_ptr<TreeNode> node) {
    std::shared_ptr<TreeNode> child = std::make_shared<TreeNode>(TreeNode("Expression"));
    node->addChild(child);
    orExpr1(child);
}

/**
 * Represents non-terminal Expr1 in the grammar
 * @param node - to add symbols to
 */
void Parser::orExpr1(std::shared_ptr<TreeNode> node) {
    andExpr1(node);
    orExpr2(node);
}

/**
 * Represents non-terminal Expr1' in the grammar
 * @param node - to add symbols to
 */
void Parser::orExpr2(std::shared_ptr<TreeNode> node) {
    if (tokens.front().getType() == TokenType::OR) {
        match(TokenType::OR, node);
        andExpr1(node);
        orExpr2(node);
    }

}

/**
 * Represents non-terminal Expr2 in the grammar
 * @param node - to add symbols to
 */
void Parser::andExpr1(std::shared_ptr<TreeNode> node) {
    equalsExpr1(node);
    andExpr2(node);
}

/**
 * Represents non-terminal Expr2' in the grammar
 * @param node - to add symbols to
 */
void Parser::andExpr2(std::shared_ptr<TreeNode> node) {
    if (tokens.front().getType() == TokenType::AND) {
        match(TokenType::AND, node);
        equalsExpr1(node);
        andExpr2(node);
    }
}

/**
 * Represents non-terminal Expr3 in the grammar
 * @param node - to add symbols to
 */
void Parser::equalsExpr1(std::shared_ptr<TreeNode> node) {
    relopExpr1(node);
    equalsExpr2(node);
}

/**
 * Represents non-terminal Expr3' in the grammar
 * @param node - to add symbols to
 */
void Parser::equalsExpr2(std::shared_ptr<TreeNode> node) {
    if (tokens.front().getType() == TokenType::EQ) {
        match(TokenType::EQ, node);
        relopExpr1(node);
        equalsExpr2(node);
    }
}

/**
 * Represents non-terminal Expr4 in the grammar
 * @param node - to add symbols to
 */
void Parser::relopExpr1(std::shared_ptr<TreeNode> node) {
    addExpr1(node);
    relopExpr2(node);
}

/**
 * Represents non-terminal Expr4' in the grammar
 * @param node - to add symbols to
 */
void Parser::relopExpr2(std::shared_ptr<TreeNode> node) {
    switch(tokens.front().getType()) {
        case TokenType::LT:
            match(TokenType::LT, node);
            break;
        case TokenType::LTE:
            match(TokenType::LTE, node);
            break;
        case TokenType::GT:
            match(TokenType::GT, node);
            break;
        case TokenType::GTE:
            match(TokenType::GTE, node);
            break;
        default:
            return;
    }


    //Only executes if token is <, <=, >, >=
    addExpr1(node);
    relopExpr2(node);
}

/**
 * Represents non-terminal Expr5 in the grammar
 * @param node - to add symbols to
 */
void Parser::addExpr1(std::shared_ptr<TreeNode> node) {
    mulExpr1(node);
    addExpr2(node);
}

/**
 * Represents non-terminal Expr5' in the grammar
 * @param node - to add symbols to
 */
void Parser::addExpr2(std::shared_ptr<TreeNode> node) {
    TokenType t = tokens.front().getType();

    if (t == TokenType::PLUS) {
        match(TokenType::PLUS, node);
    } else if (t == TokenType::MINUS) {
        match(TokenType::MINUS, node);
    } else {
        return;
    }

    //Only executes if token is + or -
    mulExpr1(node);
    addExpr2(node);
}

/**
 * Represents non-terminal Expr6 in the grammar
 * @param node - to add symbols to
 */
void Parser::mulExpr1(std::shared_ptr<TreeNode> node) {
    valueExpr(node);
    mulExpr2(node);
}

/**
 * Represents non-terminal Expr6' in the grammar
 * @param node - to add symbols to
 */
void Parser::mulExpr2(std::shared_ptr<TreeNode> node) {
    TokenType t = tokens.front().getType();

    if (t == TokenType::MULTIPLY) {
        match(TokenType::MULTIPLY, node);
    } else if (t == TokenType::DIVIDE) {
        match(TokenType::DIVIDE, node);
    } else {
        return;
    }

    //Only executes if token is * or /
    valueExpr(node);
    mulExpr2(node);
}

/**
 * Represents non-terminal Expr7 in the grammar
 * @param node - to add symbols to
 */
void Parser::valueExpr(std::shared_ptr<TreeNode> node) {
    Token t = tokens.front();

    switch(t.getType()) {
        case TokenType::NOT:
            match(TokenType::NOT, node);
            valueExpr(node);
            break;
        case TokenType::LPAREN:
            match(TokenType::LPAREN, node);
            expr(node);
            match(TokenType::RPAREN, node);
            break;
        case TokenType::STRING:
            match(TokenType::STRING, node);
            break;
        case TokenType::NUM:
            match(TokenType::NUM, node);
            break;
        case TokenType::ID:
            idExpr(node);
            break;
        case TokenType::TRUE:
            match(TokenType::TRUE, node);
            break;
        case TokenType::FALSE:
            match(TokenType::FALSE, node);
            break;
        default:
            std::string err = "Error: Invalid token ";
            err += Lexer::TOKEN_STRINGS.at(static_cast<unsigned long>(t.getType()));
            err += " on line " + std::to_string(t.getLineNum());
            err += ", character " + std::to_string(t.getColNum());
            err += ". Expected expression";
            std::cout << err << std::endl;
            throw ParseException(nullptr);
    }
}

/**
 * Represents non-terminal ID in the grammar
 * @param node - to add symbols to
 */
void Parser::idExpr(std::shared_ptr<TreeNode> node) {
    std::shared_ptr<TreeNode> temp = std::make_shared<TreeNode>(TreeNode("Temp"));
    match(TokenType::ID, temp);

    //If id is followed by left paren, must be a function call
    if (tokens.front().getType() == TokenType::LPAREN) {
        std::shared_ptr<TreeNode> child;
        child = std::make_shared<TreeNode>(TreeNode("Function Call"));
        child->addChild(temp->getChildren().front());
        node->addChild(child);
        node->setLabel("Function Call");
        funcCall(child);

    //Otherwise is simply an id
    } else {
        node->addChild(temp->getChildren().front());
    }
}
