cmake_minimum_required(VERSION 3.9)
project(CS4201_P2_Compiler)

set(CMAKE_CXX_STANDARD 17)

add_executable(CS4201_P2_Compiler src/main.cpp src/parser/Lexer.cpp
        src/parser/Parser.cpp src/parser/Token.cpp src/parser/TreeNode.cpp)