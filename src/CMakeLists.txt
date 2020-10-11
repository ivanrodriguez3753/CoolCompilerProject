#https://cmake.org/pipermail/cmake/2002-September/003028.html


cmake_minimum_required(VERSION 3.17)
project(CoolCompilerProjectSource)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_C_STANDARD 99)

message(${CMAKE_CXX_COMPILER_VERSION})

set(FLEX_OUTPUT ley.yy.c lex.yy.h)


find_package(FLEX REQUIRED)
if(FLEX_FOUND)
    message("i found flex!!")
    #correctly using gnu flex instead of apple flex
    message("Checking flex version")
    message(${FLEX_VERSION})

    #IMPORTANT NOTE: Specify the output of flex to be lex.yy.cpp instead of lex.yy.c because otherwise it will use a C
    #compiler. Right now it is generating C code that is compiled with a C++ compiler, so that we avoid undefined symbol
    #errors when including in another source file. This is because C and C++ linkage get messy and I couldn't figure out
    #the extern stuff. So just generate C code, name it with .cpp extension so FLEX_TARGET uses g++, and make sure that
    #the .l file has %option
    FLEX_TARGET(lexer lex.l ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cpp)
endif()

find_package(BISON REQUIRED)
if(BISON_FOUND)
    message("I found bison!!")
    message("Checking bison version")
    message(${BISON_VERSION})
endif()

add_library(MyLibrary Tag.h Tag.cpp ${FLEX_lexer_OUTPUTS} Token.cpp)
add_executable(CoolCompilerProject main.cpp Tag.h)
target_link_libraries(CoolCompilerProject MyLibrary)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(CoolCompilerProject
    PRIVATE
        ..
        #${FLEX_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)


