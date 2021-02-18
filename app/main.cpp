


#include <iostream>
#include "../lib/ParserDriver.h"


using namespace std;

int main() {
	cout << "Hello world!\n";



	ParserDriver drv;
    drv.parse("dummystring");
    drv.ast->prettyPrint(cout, "");
}