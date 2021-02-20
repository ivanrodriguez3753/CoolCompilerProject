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
        drv.parse(GetParam());
        drv.ast->print(actual);
        generateReference(expected, GetParam(), PARSE);
    }
};
TEST_P(astFixture, positiveCases) {
    ASSERT_EQ(actual.str(), expected.str());
}
INSTANTIATE_TEST_SUITE_P(positiveAst, astFixture, testing::Values(
    "bareMinimum.cl"
));