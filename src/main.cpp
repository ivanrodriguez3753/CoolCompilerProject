#include <iostream>
#include <ParserDriver.hh>


using namespace std;

int main(int argc, char* argv[]) {
    ParserDriver drv;
    drv.parse("../../tests/resources/CoolPrograms/bareMinimum.cl");
    drv.ast->prettyPrint(cout, "");
}