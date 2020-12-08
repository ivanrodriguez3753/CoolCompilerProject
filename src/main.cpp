#include <iostream>
#include "ParserDriver.hh"
#include "type.h"

using namespace std;

void lexInput(ParserDriver& pdrv) {
    pdrv.scan_begin();
    string tokenType = "";
    while(tokenType != "end of file") { //the token name that bison generates for <EOF> token
        yy::parser::symbol_type current{yylex(pdrv)};
        tokenType = current.name();
        if(tokenType != "end of file") {
            cout << current.location.begin.line << '\n';
            cout << current.name() << endl;

            if (tokenType == "identifier" || tokenType == "type" || tokenType == "string") {
                cout << current.value.as<string>() << endl;
            } else if (tokenType == "integer") {
                cout << current.value.as<int>() << endl;
            }
        }
    }
}



int main(int argc, char** argv) {

    string file = argv[1];
    string option = argv[2];

    if(argc != 3) {
        cout << "Incorrect number of arguments. Please enter a file name and an option." << endl;
        exit(1);
    }

    ParserDriver pdrv;

    if(option == "--lex") {
        pdrv.file = file;
        lexInput(pdrv);
    }
    else if(option == "--concreteParse") {
        pdrv.parse(file);
        pdrv.prettyPrintTree(cout);
    }
    else if(option == "--abstractParse") {
        pdrv.parse(file);
        _program* AST = (_program*) pdrv.buildSyntaxTree(rootIVAN);
        AST->prettyPrint(cout, "");
    }
    else if(option == "--semanticAnalysis") {
        pdrv.parse(file);
        _program* AST = (_program*) pdrv.buildSyntaxTree(rootIVAN);

        populateClassMap();
        populateImplementationMap();
        populateParentMap();

        AST->traverse();

        AST->prettyPrint(cout, "");
    }
}
