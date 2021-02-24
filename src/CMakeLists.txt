#https://cmake.org/pipermail/cmake/2002-September/003028.html
cmake_minimum_required(VERSION 3.17)
project(CoolCompilerProjectSource)

set(CMAKE_CXX_STANDARD 11)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
if(BISON_FOUND)
    message("I found bison!!")
    message("Checking bison version")
    message(${BISON_VERSION})

    BISON_TARGET(parser parse.yy ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
endif()
if(FLEX_FOUND)
    message("i found flex!!")
    #correctly using gnu flex instead of apple flex
    message("Checking flex version")
    message(${FLEX_VERSION})

    #IMPORTANT NOTE: Specify the output of flex to be lex.yy.cpp instead of lex.yy.c because otherwise it will use a C
    #compiler. Right now it is generating C code that is compiled with a C++ compiler, so that we avoid undefined symbol
    #errors when including in another source file. This is because C and C++ linkage get messy and I couldn't figure out
    #the extern stuff. So just generate C code, name it with .cpp extension so FLEX_TARGET uses g++, and make sure that
    #the .l file has %option header-file="lex.yy.h" or whatever you want to call it
    FLEX_TARGET(lexer lex.ll ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
endif()

add_flex_bison_dependency(lexer parser)

add_library(MyLibrary ${FLEX_lexer_OUTPUTS} ${BISON_parser_OUTPUTS} ParserDriver.hh ParserDriver.cpp ast.h ast.cpp)
add_executable(CoolCompilerProject main.cpp ${FLEX_lexer_OUTPUTS} ${BISON_parser_OUTPUTS} ParserDriver.hh ParserDriver.cpp ast.h ast.cpp)
target_link_libraries(CoolCompilerProject MyLibrary stdc++)

target_include_directories(CoolCompilerProject
    PUBLIC
        ..
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_directories(CoolCompilerProject PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})


