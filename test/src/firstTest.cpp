#include "gtest/gtest.h"
#include "utility.h"
#include <ParserDriver.h>

using namespace std;

TEST(someSuite, test1) {
    cout << "Hello world!\n";
    ParserDriver drv;
    drv.parse("dummystring");
    drv.ast->prettyPrint(cout, "");



    ASSERT_TRUE(true);
}