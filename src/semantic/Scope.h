#pragma once


#include <vector>
#include <string>
#include <map>
#include <memory>

enum class Object {VAR, PROC};
enum class Type {INT, BOOL, STRING};
enum class Block {GLOBAL, IF, ELSE, WHILE, PROC};

class Scope {
private:
    std::shared_ptr<Scope> parent;
    Block block;
    bool global;
    std::vector<std::shared_ptr<Scope>> scopes;
    std::map<std::string, std::pair<Object, Type>> symbolTable;
public:
    Scope();
    Scope(const Scope& parent, Block block);

    const std::vector<std::shared_ptr<Scope>> &getScopes() const;

    const std::map<std::string, std::pair<Object, Type>> &getSymbolTable() const;

    void addSymbol(std::string id, Object obj, Type type);

    void addScope(Block block);

    bool inScope(std::string id, Object obj);

    bool declared(std::string id);

    bool isGlobal() const;

    Block getBlock() const;
};


