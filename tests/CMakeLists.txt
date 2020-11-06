cmake_minimum_required(VERSION 3.17)
project(CoolCompilerProjectTests)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/../src/lex.yy.cpp GENERATED)

message("hiiii")
message(${CMAKE_CURRENT_SOURCE_DIR})
add_executable(
    tests
        src/lexerTests.cpp
    src/main.cpp
    #insert .cpp test files here, like src/tests.cpp
        src/parseTreeTests.cpp src/syntaxTreeTests.cpp )

target_include_directories(
    tests
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src #test source code
    ${CMAKE_CURRENT_SOURCE_DIR}/../src #main project source code
)

target_include_directories(tests
        PRIVATE
        #the tests
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        #${FLEX_INCLUDE_DIRS}
        #the generated ley.yy.cpp and lex.yy.h
        ${CMAKE_CURRENT_BINARY_DIR}/../src
        #other source files like Tag.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../
)


#link against stuff from main source code
add_dependencies(tests CoolCompilerProject)

#Link against Google Test library
target_link_libraries(tests gmock_main gmock gtest MyLibrary)
target_link_directories(tests PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/../src)

#Build Google Test library
add_subdirectory(vendor)
