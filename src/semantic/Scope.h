#pragma once


#include <vector>
#include <string>
#include <map>
#include <memory>
#include "../parser/TreeNode.h"

//Distinguishes between variables, parameters and functions in symbol table
enum class Object {VAR, PARAM, PROC};

//Used to denote variable types and return types
enum class Type {INT, BOOL, STRING, NONE};

//Used to identify the type of scope
enum class Block {GLOBAL, IF, ELSE, WHILE, PROC};

/**
 * Represents a node in the tree of scopes
 */
class Scope {
private:
    //Stores list of identifiers in order they were added
    std::vector<std::pair<std::string, std::pair<Object, Type>>> identifiers;

    //Current scope index
    unsigned long current = 0;

    //Parent scope
    std::shared_ptr<Scope> parent;

    //Type of scope
    Block block;

    //If the scope is global
    bool global;

    //List of children scopes (e.g. conditional statements in a function)
    std::vector<std::shared_ptr<Scope>> scopes;

    //Symbol table containing [id, object, type] e.g. [x, var, int]
    std::map<std::string, std::pair<Object, Type>> symbolTable;
public:
    //Constructor for global scope
    Scope();

    //Constructor for child scopes
    Scope(const Scope& parent, Block block);

    //Getter for list of child scopes
    const std::vector<std::shared_ptr<Scope>> &getScopes() const;

    //Getter for symbol table
    const std::map<std::string, std::pair<Object, Type>> &getSymbolTable() const;

    //Gets a symbol from the current symbol table or a parent scope
    const std::pair<Object, Type>& getSymbol(std::string id, const std::shared_ptr<Scope>& scope) const;

    //Adds a symbol to the symbol table of the current scope
    void addSymbol(std::string id, Object obj, Type type);

    //Adds a scope to the list of child scopes
    void addScope(Block block);

    //Returns whether a variable/function is in scope (i.e. declared in current scope or higher)
    bool inScope(std::string id, Object obj);

    //Returns whether a symbol has been declared in the current scope
    bool declared(std::string id);

    //Returns whether the current scope is the global scope
    bool isGlobal() const;

    //Getter for the scope/block type
    Block getBlock() const;

    //Gets current index of scope to traverse
    unsigned long getCurrent() const;

    //Sets current index of scope to traverse
    void setCurrent(unsigned long current);

    const std::vector<std::pair<std::string, std::pair<Object, Type>>> getFuncIDs(const std::string& funcID, const std::shared_ptr<Scope>& scope);

    //Gets the type of return value required by the context of a return statement
    Type getReturnType(const std::shared_ptr<Scope>& scope);

    //Sets parent scope node
    void setParent(const std::shared_ptr<Scope> &parent);
};


