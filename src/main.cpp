#include <iostream>
#include <ParserDriver.hh>
#include <fstream>
#include <sstream>


using namespace std;

void writeLLFile(ParserDriver& drv, string fileName) {
    fileName.replace(fileName.find(".cl"), 3, ".ll");
    ofstream llFile(fileName);

    string llContents;
    llvm::raw_string_ostream ostr(llContents);
    drv.llvmModule->print(ostr, nullptr);

    llFile << stringstream(llContents).rdbuf();
    llFile.close();
}

int main(int argc, char* argv[]) {
    ParserDriver drv;
    string fileName;
    bool printAST = false;

    if(argc == 3) {
        string s = argv[1];
        if(s != "--ast") {
            cerr << "Only supported option is --ast" << endl;
        }
        else {
            fileName = argv[2];
            printAST = true;
        }
    }
    else if(argc == 2) {
        fileName = argv[1];
    }



    drv.parse(fileName);
    if(printAST) {
        drv.ast->prettyPrint(cout, "");
    }

    drv.buildInternalsAst();
    drv.buildClassAndMethodEnvs();
    drv.populateClassImplementationMaps();
    drv.decorateAST();

    if(drv.errorLog.size()) {
        for(auto entry : drv.errorLog) {
            cerr << "ERROR: " + to_string(entry.first) + ": " + entry.second << endl;
        }
        cout << "Halted before codegen due to outstanding errors from the type checker\n";
        exit(1);
    }

    drv.codegen();
    writeLLFile(drv, fileName);
}