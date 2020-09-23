#include <iostream>
#include <lex.yy.h>

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
    while(yylex()) {
        cout << yytext << endl;
    }

}
