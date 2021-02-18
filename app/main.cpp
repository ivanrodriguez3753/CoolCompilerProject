


#include <iostream>
#include <ParserDriver.h>


using namespace std;

int main() {
	cout << "Hello world!\n";



	ParserDriver drv;
    drv.parse("dummystring");
    drv.ast->prettyPrint(cout, "");
}