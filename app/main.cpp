


#include <iostream>
#include <ParserDriver.h>


using namespace std;

int main(int argc, char* argv[]) {

	ParserDriver drv;
    drv.parse("../test/resources/CoolPrograms/bareMinimum.cl");
    drv.ast->prettyPrint(cout, "");
}