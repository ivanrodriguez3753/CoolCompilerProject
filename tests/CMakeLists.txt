cmake_minimum_required(VERSION 3.17)
project(CoolCompilerProject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#Build Google Test library
add_subdirectory(vendor)

add_executable(
    tests
    src/lexer.cpp
    src/main.cpp
    ../../Tag.cpp
    ../../cmake-build-debug/lex.yy.cpp
    #insert .cpp test files here, like src/tests.cpp
)
add_test(NAME aName COMMAND tests)

target_include_directories(
    tests
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src #test source code
    ${CMAKE_CURRENT_SOURCE_DIR}/../src #main project source code
)

target_include_directories(tests
        PRIVATE
        #the tests
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        #${FLEX_INCLUDE_DIRS}
        #the generated ley.yy.cpp and lex.yy.h
        ${CMAKE_CURRENT_BINARY_DIR}/../
        #other source files like Tag.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../
)


#Link against Google Test library
target_link_libraries(tests gmock_main gmock gtest)