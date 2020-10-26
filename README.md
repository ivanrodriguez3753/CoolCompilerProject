To build, clone the repo then in the root of the repo:

mkdir build <br />
cd build <br />
cmake .. <br />
make <br />

To run the tests, navigate to the tests build directory and run the executable named tests. Continuing from above:
cd tests <br />
./tests <br />

After building, Compiler can be run as follows <br />
cd src <br />
./CoolCompilerProject \<fileName\> \<option\> <br />

Currently, there are two options: --lex and --concreteParse. The former will produce give a lexical analysis and the latter will give a concrete parse tree of the input, printed in a human readable format.
