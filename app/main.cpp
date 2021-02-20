


#include <iostream>
#include <ParserDriver.h>


using namespace std;

int main() {
	cout << "Hello world!\n";



	ParserDriver drv;
    drv.parse("bareMinimum.cl");
    drv.ast->prettyPrint(cout, "");
}