//
// Created by Ivan Rodriguez on 2/20/21.
//

#ifndef COOLCOMPILERPROJECT_UTILITY_H
#define COOLCOMPILERPROJECT_UTILITY_H

#include <fstream>
#include <string>

using namespace std;

extern const string buildToResourcesPath;

/**
 * the test executable is run from /build
 * @param expected
 * @param fileName
 * @param compilerStage
 * @return
 */
void generateReference(ostream& expected, const string fileName, const string compilerStage);
#endif //COOLCOMPILERPROJECT_UTILITY_H
