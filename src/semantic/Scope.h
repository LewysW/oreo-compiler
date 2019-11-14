#pragma once


#include <vector>
#include <string>
#include <map>
#include <memory>

enum class Object {VAR, PROC};
enum class Type {INT, BOOL, STRING};
enum class Block {IF, ELSE, WHILE, PROC};

class Scope {
private:
    std::shared_ptr<Scope> parent;
    bool global;
    std::vector<std::shared_ptr<std::pair<Block, Scope>>> scopes;
    std::map<std::string, std::pair<Object, Type>> symbolTable;
public:
    Scope();
    Scope(Scope& const parent);

    const std::vector<std::shared_ptr<std::pair<Block, Scope>>> &getScopes() const;

    const std::map<std::string, std::pair<Object, Type>> &getSymbolTable() const;

    void addSymbol(std::string id, Object obj, Type type);

    void Scope::addScope(Block block);

    bool inScope(std::string id);

    bool declared(std::string id);

    bool isGlobal() const;
};


