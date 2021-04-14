cmake_minimum_required(VERSION 3.16)

project(CoolCompilerProjectAll)

add_subdirectory(src)




#comment or uncomment for testing
include(CTest)
enable_testing()
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

add_test(NAME MyTest COMMAND Test)