
#include "ParserDriver.hh"
#include "parser.hh"

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

void ParserDriver::scan_begin() {

}

void ParserDriver::scan_end() {

}
