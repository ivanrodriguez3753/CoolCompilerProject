#include <cstdlib>
#include <iostream>
#include <string>
#include "parse.hpp"
#include "lexer.yy.hpp"
#include "ast.h"
#include "omp.h"
#include "ParserDriver.h"


using namespace std;

//void* ParseAlloc(void* (*allocProc)(size_t));
//void Parse(void* parser, int token, const char* tokenInfo, ParserDriver* info);
//void ParseFree(void* parser, void(*freeProc)(void*));
 



 
int main() {
    ParserDriver drv;
    drv.parse("notARealFile.txt");

//#pragma omp parallel num_threads(1)
//{
//    printf("hello world!\n");
//}

    drv.ast->prettyPrint(cout, "");
}
