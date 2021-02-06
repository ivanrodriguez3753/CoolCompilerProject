#include <fstream>
#include <regex>
#include "gtest/gtest.h"
#include "parser.hh"
#include "ParserDriver.hh"
#include "type.h"
#include "codegen.h"
using namespace std;

/**
 * Currently, the googletest tests executable is in /CoolCompilerProject/cmake-build-debug/tests. Going to root
 * of project is two directories back
 */
const string tests_EXE_TO_ROOT = "../../";
const string RESOURCES_DIR_FROM_ROOT = "tests/resources/";
const string COOL_ASSEMBLY_PROGRAMS_DIR_FROM_RESOURCES = "CoolAssemblyPrograms/";
const string tests_EXE_TO_COOL_ASSEMBLY_PROGRAMS = tests_EXE_TO_ROOT + RESOURCES_DIR_FROM_ROOT + COOL_ASSEMBLY_PROGRAMS_DIR_FROM_RESOURCES;
const string CD = "cd ";
const string outputOption = "--out ";

void generateRefAndIvanTemps(stringstream& expected, stringstream& actual, string fileName) {
    vector<string> returnThis;
    //generate the reference output
    string command = CD + tests_EXE_TO_ROOT + RESOURCES_DIR_FROM_ROOT + " && ./cool " +  COOL_ASSEMBLY_PROGRAMS_DIR_FROM_RESOURCES + fileName + " >> refTemp.out";
    system(command.c_str());


    //read reference output into an ifstream
    ifstream ifsRef(tests_EXE_TO_ROOT + RESOURCES_DIR_FROM_ROOT + "refTemp.out");

    //make it a stringstream so we can compare
    expected << ifsRef.rdbuf();
    ifsRef.close();

    const string clasmFile = "ivanTemp.cl-asm";
    ofstream ofs(tests_EXE_TO_ROOT + RESOURCES_DIR_FROM_ROOT + clasmFile);
    for(string instr : code) {
        ofs << instr << endl;
    }
    ofs.close();


    command = CD + tests_EXE_TO_ROOT + RESOURCES_DIR_FROM_ROOT + " && ./cool " + clasmFile + " >> ivanTemp.out";
    system(command.c_str());

    //read ivanOutput into an ifstream
    ifstream ifsIvan(tests_EXE_TO_ROOT + RESOURCES_DIR_FROM_ROOT + "ivanTemp.out");
    actual << ifsIvan.rdbuf();
    ifsIvan.close();
}

void removeRefAndIvanTemps() {
    string removeCommand = CD + tests_EXE_TO_ROOT + RESOURCES_DIR_FROM_ROOT + " && rm refTemp.out ivanTemp.out ivanTemp.cl-asm";
    system(removeCommand.c_str());
}

class codeGen : public testing::TestWithParam<string> {
protected:
    ParserDriver pdrv;
    _program* AST;

    stringstream expected, actual;

    void SetUp() override {
        //parse the input into global parse tree rootIVAN
        pdrv.parse(tests_EXE_TO_COOL_ASSEMBLY_PROGRAMS + GetParam());
        //build syntax tree out of rootIVAN
        AST = (_program*) pdrv.buildSyntaxTree(rootIVAN);
        populateClassMap();
        populateImplementationMap();
        populateParentMap();

        AST->traverse();
        AST->codeGen();

        generateRefAndIvanTemps(expected, actual, GetParam());
    }

    void TearDown() override {
        removeRefAndIvanTemps();

        classMap.clear(); classMapOrdered.clear();
        implementationMap.clear(); implementationMapOrdered.clear();
        parentMap.clear();
        globalEnv->reset();
        code.clear();
        expected.clear(); actual.clear();
    }
};
TEST_P(codeGen, errorsNotInReferenceCompiler) {
    ASSERT_EQ(expected.str(), actual.str());
}
INSTANTIATE_TEST_SUITE_P(codeGenEndToEnd, codeGen, testing::Values(
        "PA5YouTubeExample.cl",
        "methodsDifferentNumParameters.cl",
        "methodsDifferentNumParametersLet.cl",
        "methodsDifferentNumParametersLetWithInitializer.cl",
        "methodsDifferentNumParametersLetWithInitializers.cl",
        "chainedCopy.cl",
        "dynamicDispatchOffAnonymous.cl",
        "if.cl",
        "isvoid.cl",
        "relational.cl",
        "unary.cl",
        "assign.cl"
));
