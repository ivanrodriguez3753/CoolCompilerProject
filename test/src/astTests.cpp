#include "gtest/gtest.h"
#include "utility.h"
#include <ParserDriver.h>


using namespace std;

class astFixture : public testing::TestWithParam<string> {
protected:
    ParserDriver drv;
    const string PARSE = "--parse";

    stringstream expected, actual;

    void SetUp() override {
        generateReference(expected, GetParam(), PARSE);
    }
};
TEST_P(astFixture, positiveCases) {
    drv.parse(buildToResourcesPath + "CoolPrograms/" + GetParam());
    drv.ast->print(actual);

    ASSERT_EQ(actual.str(), expected.str());
}
INSTANTIATE_TEST_SUITE_P(positiveAst, astFixture, testing::Values(
    "bareMinimum.cl",
    "classInherits.cl",
    "singleAttrInit.cl",
    "singleAttrNoInit.cl",
    "methodOneFormal.cl",
    "methodTwoFormals.cl",
    "methodFiveFormals.cl",
    "severalAttrAndMethods.cl",
    "integerExpr.cl"
));