#include "gtest/gtest.h"
#include "utility.h"
#include "codegen.h"

#include "ParserDriver.hh"

class codegenFixture : public testing::TestWithParam<string> {
protected:

    stringstream expected, actual;

    void SetUp() override {

    }
    void TearDown() override {

    }
};
TEST_P(codegenFixture, positive) {
    ParserDriver drv;

    drv.parse(buildToResourcesPath + "CoolPrograms/" + GetParam());
    drv.buildInternalsAst();
    drv.buildEnvs();
    drv.populateClassImplementationMaps();

    drv.internalsAst->decorateInternals(drv.env);
    drv.ast->decorate(drv);

    codegenDriver cdrv(drv);
}
INSTANTIATE_TEST_SUITE_P(positiveCodegen, codegenFixture, testing::Values(
        "bareMinimum.cl"
));
