#include <iostream>
#include <vector>
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
    identifiers.emplace_back(std::make_pair(id, std::make_pair(obj, type)));
}

/**
 * Adds a scope of type Block to the list of child scopes
 * @param block - to add
 */
void Scope::addScope(Block blockVal) {
    Scope parentScope = *this;
    Scope child(parentScope, blockVal);
    std::shared_ptr<Scope> scopePtr = std::make_shared<Scope>(child);

    scopes.emplace_back(scopePtr);
}

/**
 * Returns a lit of symbols for a given function
 * @param funcID - function identifier
 * @param scope - to search
 * @return - sequential list of function symbols
 */
const std::vector<std::pair<std::string, std::pair<Object, Type>>> Scope::getFuncIDs(const std::string& funcID,
                                                                                     const std::shared_ptr<Scope>& scope) {
    //If function is declared in current scope
    if (declared(funcID)) {
        int numFuncs = 0;
        //Find position of function in scope
        for (const std::pair<std::string, std::pair<Object, Type>> id : scope->identifiers) {
            if (scope->symbolTable[id.first].first == Object::PROC) {
                numFuncs++;
                if (id.first == funcID) break;
            }
        }

        int currentFunc = 0;
        //Get identifiers of scope at that position and return them
        for (const std::shared_ptr<Scope>& s : scope->getScopes()) {
            if (s->block == Block::PROC) {
                currentFunc++;

                if (currentFunc == numFuncs) {
                    return s->identifiers;
                }
            }
        }
    }

    return getFuncIDs(funcID, scope->parent);
}

/**
 * Checks if an object is in scope
 * @param id - of object to check
 * @param obj - whether object is variable or function
 * @return whether object is in scope
 */
bool Scope::inScope(std::string id, Object obj) {
    int inScope;
    //If symbol is not in table
    if (symbolTable.find(id) == symbolTable.end()) {
        //Return false if in global scope, or check parent scope
        return (isGlobal()) ? false : parent->inScope(id, obj);
    }

    Object entry = (symbolTable[id].first == Object::PROC) ? Object::PROC : Object::VAR;

    if (entry != obj) {
        return false;
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

/**
 * Returns current scope being traversed
 */
unsigned long Scope::getCurrent() const {
    return current;
}

/**
 * Sets index of current scope
 * @param current - index of current scope
 */
void Scope::setCurrent(unsigned long current) {
    Scope::current = current;
}

/**
 * Gets the return type of the current scope,
 * i.e. if return statement is in function, gets function return type,
 * otherwise if global scope, returns type INT (which denotes an exit code)
 * @param scope - to traverse for return type
 * @return return type of current scope
 */
Type Scope::getReturnType(const std::shared_ptr<Scope>& scope) {
    //Return type is INT in global scope for status code
    if (scope->isGlobal()) {
        return Type::INT;
    } else if (scope->block == Block::PROC) {
        unsigned long funcPos = 0;

        //Record number of functions within parent scope
        for (const std::shared_ptr<Scope>& s : scope->parent->getScopes()) {
            //If current scope equals child scope of parent, increment number of functions and break (scope found)
            if (s == scope) {
                //Increment the number of
                funcPos++;
                break;
            //otherwise if other function scope found, increment number of functions
            } else if (s->block == Block::PROC){
                funcPos++;
            }
        }

        //Find ID corresponding to function position
        unsigned long idPos = 0;
        for (const std::pair<std::string, std::pair<Object, Type>>& id : parent->identifiers) {
            //If identifier in parent is a procedure, increment identifier position
            if (parent->getSymbol(id.first, parent).first == Object::PROC) {
                idPos++;

                //If ID corresponds to function position, return type of function from symbol table
                if (idPos == funcPos) {
                    return parent->getSymbol(id.first, scope).second;
                }
            }
        }
    }

    //Otherwise if current scope is not procedure or global, check parent block
    return getReturnType(scope->parent);
}

/**
 * Setter for scope parent
 * @param parent - parent node of scope
 */
void Scope::setParent(const std::shared_ptr<Scope> &parent) {
    Scope::parent = parent;
}


