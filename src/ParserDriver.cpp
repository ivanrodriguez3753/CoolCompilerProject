
#include "ParserDriver.hh"
#include "parser.hh"
#include "parseTree/parseTreeNodes.h"
ParserDriver::ParserDriver() : trace_parsing{false}, trace_scanning{false} {}

int ParserDriver::parse(const std::string& f) {
    file = f;
    location.initialize(&file);
    scan_begin();
    yy::parser parse(*this);
    parse.set_debug_level(trace_parsing);
    int res = parse();
    scan_end();
    return res;
}

void ParserDriver::postorderTraversal() {
    postorderRecurs(rootIVAN);
}

void ParserDriver::postorderRecurs(node *current) {
    for(auto child : *current->children) {
        postorderRecurs(child);
    }
    if(current->children->empty()) { //terminal node
        terminalNode* term = (terminalNode*)current;
        cout << term->tokenType << endl;
    }

}


//void ParserDriver::scan_begin() {
//    yy_flex_debug = trace_scanning;
//    if (file.empty () || file == "-")
//        yyin = stdin;
//    else if (!(yyin = fopen (file.c_str (), "r")))
//    {
//        std::cerr << "cannot open " << file << ": " << strerror (errno) << '\n';
//        exit (EXIT_FAILURE);
//    }
//}

//void ParserDriver::scan_end() {
//
//}
