
#include "ParserDriver.hh"
#include "parser.hh"
#include "parseTreeNodes.h"
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

/**
 * Driver method for testing the parse tree. Prints the input file by traversing the parse tree
 * and printing leaf nodes (terminals).
 */
void ParserDriver::postorderTraversal(ostream& out) {
    postorderRecurs(rootIVAN, out);
}

void ParserDriver::postorderRecurs(node *current, ostream& out) {
    for(auto child : *current->children) {
        postorderRecurs(child, out);
    }
    if(current->children->empty()) { //node with no children
        if(current->isTerminal) { //we need this because we can have empty lists, which we don't want to print anything for
            terminalNode *term = (terminalNode *) current;
            if (term->grammarSymbol == "identifier" || term->grammarSymbol == "type" || term->grammarSymbol == "string") {
                wordNode *word = (wordNode *) term;
                out << word->value << endl;
            } else {
                if(tokenReqTranslation.find(term->grammarSymbol) != tokenReqTranslation.end()) {
                    out << tokenReqTranslation[term->grammarSymbol] << endl;
                }
                else {
                    out << term->grammarSymbol << endl;
                }
            }
        }
    }

}

//taken and adapted from https://stackoverflow.com/questions/36802354/print-binary-tree-in-a-pretty-way-using-c
void ParserDriver::prettyPrintTree(ostream& out) {
    prettyPrintRecurs(rootIVAN, "", out);
}

//uses preorder traversal strategy
void ParserDriver::prettyPrintRecurs(node* current, const string& prefix, ostream& out) {
    if(current != nullptr) {
        out << prefix;
        out << "├──";

        //print the node
        out << current->grammarSymbol << endl;

        //enter the next tree level
        for(auto child : *current->children) {
            string newPrefix = prefix + "|   ";
            prettyPrintRecurs(child, newPrefix, out);
        }


    }
}

/**
 * This is a global map for tokens whose name differ from its lexeme
 */
map<string, string> tokenReqTranslation{
    {"at", "@"},
    {"colon", ":"},
    {"comma", ","},
    {"divide", "/"},
    {"dot", "."},
    {"equals", "="},
    {"larrow", "<-"},
    {"lbrace", "{"},
    {"le", "<="},
    {"lparen", "("},
    {"lt", "<"},
    {"minus", "-"},
    {"plus", "+"},
    {"rarrow", "->"},

    {"rbrace", "}"},
    {"rparen", ")"},
    {"semi", ";"},
    {"tilde", "~"},
    {"times", "*"}
};
