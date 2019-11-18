#pragma once


#include <vector>
#include <string>
#include <map>
#include <memory>

//Distinguishes between variables and functions in symbol table
enum class Object {VAR, PROC};

//Used to denote variable types and return types
enum class Type {INT, BOOL, STRING, NONE};

//Used to identify the type of scope
enum class Block {GLOBAL, IF, ELSE, WHILE, PROC};

/**
 * Represents a node in the tree of scopes
 */
class Scope {
private:
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

    //Adds a symbol to the symbol table of the current scope
    void addSymbol(std::string id, Object obj, Type type);

    //Adds a scope to the list of child scopes
    void addScope(Block block);

    //Returns whether a variable/function is in scope (i.e. declared in current scope or higher)
    bool inScope(std::string id, Object obj);

    //Returns whether a variable/function has been declared in the current scope
    bool declared(std::string id, Object obj);

    //Returns whether the current scope is the global scope
    bool isGlobal() const;

    //Getter for the scope/block type
    Block getBlock() const;
};


