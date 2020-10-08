cmake_minimum_required(VERSION 3.17)
project(CoolCompilerProject)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_C_STANDARD 99)

message(${CMAKE_CXX_COMPILER_VERSION})

set(FLEX_OUTPUT ley.yy.c lex.yy.h)


find_package(FLEX REQUIRED)
if(FLEX_FOUND)
    message("i found flex!!")
    message(${CMAKE_BINARY_DIR})
    #correctly using gdb flex instead of apple flex
    message("Checking flex version")
    message(${FLEX_VERSION})


    #IMPORTANT NOTE: Specify the output of flex to be lex.yy.cpp instead of lex.yy.c because otherwise it will use a C
    #compiler. Right now it is generating C code that is compiled with a C++ compiler, so that we avoid undefined symbol
    #errors when including in another source file. This is because C and C++ linkage get messy and I couldn't figure out
    #the extern stuff. So just generate C code, name it with .cpp extension so FLEX_TARGET uses g++, and make sure that
    #the .l file has %option
    FLEX_TARGET(lexer lex.l ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cpp)
endif()



add_executable(CoolCompilerProject main.cpp Token.cpp ${FLEX_lexer_OUTPUTS} Tag.h Tag.cpp)

target_include_directories(CoolCompilerProject
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        #${FLEX_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
)


#Comment/uncomment this for testing
include(CTest)
enable_testing()
if(BUILD_TESTING)
    message("entered BUILD_TESTING")
    add_subdirectory(tests)
endif()