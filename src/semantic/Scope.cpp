#include "Scope.h"

/**
 * Getter for list of child scopes in current scope
 * @return list of scopes
 */
const std::vector<std::shared_ptr<Scope>> &Scope::getScopes() const {
    return scopes;
}

/**
 * Getter for symbol table of scope
 * @return symbol table map
 */
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
    Scope child(parent, block);
    std::shared_ptr<Scope> scopePtr = std::make_shared<Scope>(child);

    scopes.emplace_back(scopePtr);
}

/**
 * Checks if an object is in scope
 * @param id - of object to check
 * @param obj - whether object is variable or function
 * @return whether object is in scope
 */
bool Scope::inScope(std::string id, Object obj) {
    //If symbol is not in table
    if (symbolTable.find(id) == symbolTable.end() || symbolTable[id].first != obj) {
        //Return false if in global scope, or check parent scope
        return (isGlobal()) ? false : parent->inScope(id, obj);
    }

    //Otherwise object is in scope
    return true;
}

/**
 * Checks if an object was declared in the current scope
 * @param id - of object to check
 * @return whether object is declared in current scope
 */
bool Scope::declared(std::string id, Object obj) {
    return (symbolTable.find(id) != symbolTable.end() && symbolTable[id].first == obj);
}

/**
 * Default constructor for global scope
 */
Scope::Scope() : parent(nullptr),
                 global(true),
                 block(Block::GLOBAL)
{
}

/**
 * Constructor for child scopes
 * @param parent
 */
Scope::Scope(const Scope& parent, Block block) : parent(std::make_shared<Scope>(parent)),
                                    global(false),
                                    block(block)
{
}

/**
 * Global variable denotes whether current scope is global
 * @return
 */
bool Scope::isGlobal() const {
    return global;
}

/**
 * Getter for type of block
 * @return block of scope
 */
Block Scope::getBlock() const {
    return block;
}

/**
 * Gets the object and type of a given symbol from within the current scope
 * @param id - of symbol to find
 * @param scope - to search
 * @return - pair of object and type
 */
const std::pair<Object, Type> &Scope::getSymbol(const std::string id, const std::shared_ptr<Scope>& scope) const {
    if (scope->symbolTable.find(id) != scope->symbolTable.end()) {
        return scope->symbolTable.at(id);
    }

    return getSymbol(id, scope->parent);
}


