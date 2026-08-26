cmake_minimum_required(VERSION 3.20)
project(nathlang)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(nathlang
    src/main.cpp
    src/Nathlang.cpp
    src/Interpreter.cpp
    src/Scanner.cpp
    src/ErrorHandler.cpp
    src/Parser.cpp
    src/AstPrinter.cpp
    src/Environment.cpp
    src/Function.cpp
    src/Resolver.cpp
    src/Class.cpp
    src/Instance.cpp
    src/StdLib.cpp
)

target_include_directories(nathlang PRIVATE
    src
)
