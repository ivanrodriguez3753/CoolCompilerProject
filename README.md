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

Currently, there are three options: --lex, --concreteParse, and --abstractParse. --lex will produce give a lexical analysis, --concreteParse will give a concrete parse tree of the input printed in a human readable format, and --abstractParse will give an output that matches the reference lexer's AST specification. Working on human readable format similar to --concreteParse.
