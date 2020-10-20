#include <iostream>
#include "ParserDriver.hh"

using namespace std;

int main(int argc, char** argv) {
//    if(argc != 2) {
//        cout << "Please provide a .cl source file\n";
//    }
//    FILE* inputFile = fopen(argv[1], "r");
//
//    if(!inputFile) {
//        cout << "Couldn't find input file!" << endl;
//        return 1;
//    }
    int res = 0;
    ParserDriver drv;
    for(int i = 1; i < argc; ++i) {
        if(argv[i] == std::string("-p")) {
            drv.trace_parsing = true;
        }
        else if(argv[i] == std::string("-s")) {
            drv.trace_scanning = true;
        }
        else if(!drv.parse(argv[i])) {
            std::cout << drv.result << '\n';
        }
        else {
            res = 1;
        }
    }
    drv.postorderTraversal();
    return res;



}
