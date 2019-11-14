#include "Scope.h"

const std::vector<std::shared_ptr<std::pair<Block, Scope>>> &Scope::getScopes() const {
    return scopes;
}

const std::map<std::string, std::pair<Object, Type>> &Scope::getSymbolTable() const {
    return symbolTable;
}

/**
 * Adds a symbol to the symbol table in the form <id, <obj, type>>
 * @param id - of symbol to be added
 * @param obj - (variable or procedure)
 * @param type - of variable or return type of procedure
 */
void Scope::addSymbol(std::string id, Object obj, Type type) {
    std::pair<Object, Type> value = std::make_pair(obj, type);
    symbolTable.insert(std::pair<std::string, std::pair<Object, Type>>(id, value));
}

void Scope::addScope(Block block) {
    Scope parent = *this;
    Scope child(parent);
    std::pair p = std::make_pair(block, child);
    std::shared_ptr<std::pair<Block, Scope>> scopePtr = std::make_shared(p);

    scopes.emplace_back(scopePtr);
}

/**
 * Checks if an object is in scope
 * @param id - of object to check
 * @return whether object is in scope
 */
bool Scope::inScope(std::string id) {
    //If symbol is not in table
    if (symbolTable.find(id) == symbolTable.end()) {
        //Return false if in global scope, or check parent scope
        return (isGlobal()) ? false : parent->inScope(id);
    }

    //Otherwise object is in scope
    return true;
}

/**
 * Checks if an object was declared in the current scope
 * @param id - of object to check
 * @return whether object is declared in current scope
 */
bool Scope::declared(std::string id) {
    return (symbolTable.find(id) != symbolTable.end());
}

/**
 * Default constructor for global scope
 */
Scope::Scope() : parent(nullptr),
                 global(true)
{
}

/**
 * Constructor for child scopes
 * @param parent
 */
Scope::Scope(Scope& const parent) : parent(std::make_shared(parent)),
                                    global(false)
{
}

/**
 * Global variable denotes whether current scope is global
 * @return
 */
bool Scope::isGlobal() const {
    return global;
}


