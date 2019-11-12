#pragma once


#include <vector>
#include <string>
#include <map>
#include <memory>

enum class Object {VAR, PROC};
enum class Type {INT, BOOL, STRING};

class Scope {
private:
    std::vector<std::shared_ptr<Scope>> scopes;
    std::map<std::string, std::pair<Object, Type>> symbolTable;
};


