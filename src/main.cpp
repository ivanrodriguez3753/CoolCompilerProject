#include <iostream>
#include "Tag.h"
#include "lex.yy.h"

using namespace std;

int main(int argc, char** argv) {
    if(argc != 2) {
        cout << "Please provide a .cl source file\n";
    }
    FILE* inputFile = fopen(argv[1], "r");

    if(!inputFile) {
        cout << "Couldn't find input file!" << endl;
        return 1;
    }
    yyin = inputFile;

    cout.setf(std::ios::unitbuf);

    int token;
    while( (token = yylex()) ) {
        if(token != 999) {
            cout << yylineno << endl << tagMap.at(token) << endl;
            if(token == IDENTIFIER || token == STRING || token == TYPE || token == INTEGER) {
                cout << yylval->lexeme << endl;
            }
        }

    }



}
