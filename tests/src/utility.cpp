#include "utility.h"




using namespace std;
const string buildToResourcesPath = "../../tests/resources/";

//TODO: enumerate later but compilerStage is one of "--parse", "--type", "full"
void generateReference(ostream& o, const string fileName, const string compilerStage) {
    string refTemp = "refTemp.txt";

    string genref_cmd = "cd " + buildToResourcesPath + " && ./cool ";
    if(compilerStage == "--parse" || compilerStage == "--type") {
        genref_cmd += compilerStage + ' ';
    }
    genref_cmd += "CoolPrograms/" + fileName;

    string pwd = "pwd";
    system(pwd.c_str());
    system(genref_cmd.c_str());

    string generatedFile = buildToResourcesPath + "CoolPrograms/" + fileName;
    if(compilerStage == "--parse") generatedFile += "-ast";
    else if(compilerStage == "--type") generatedFile += "-type";
    else if(compilerStage == "--full") generatedFile += "-out";

    ifstream ifsRef(generatedFile);
    o << ifsRef.rdbuf();
    ifsRef.close();
};

void writeMyOutputToCoolProgramsDir(string actualOutput, string fileName) {
    ofstream ofs(buildToResourcesPath + "CoolPrograms/" + fileName);
    ofs << actualOutput;
}