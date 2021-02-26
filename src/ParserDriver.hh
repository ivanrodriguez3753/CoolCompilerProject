/*
 * This file provides control mechanisms for the parser, which uses the lexer. Follows the tutorial for the
 * complete C++ parser in Chapter10 of the bison documentation
 */

#ifndef PARSERDRIVER_H
#define PARSERDRIVER_H


#include <map>
#include <string>
#include <set>
#include "ast.h"
#include "parser.hpp"
#include "environment.h"


//give Flex the prototype of yylex we want...
#define YY_DECL yy::parser::symbol_type yylex(ParserDriver& drv)
//...and declare it for the parser's sake
YY_DECL;

using namespace std;

class ParserDriver {
public:
    int encountered = 0;

    _program* bisonProduct;

    _program* ast;
    _program* internalsAst;
    globalEnv* env;

    map<string, set<pair<objRec*, int>>> classMap;

    /**
     * methodRec* has info about defining class, and int is the position. Don't change the position if the child overrides
     * a parent's method, only change the methodRec*
     */
    map<string, map<string, pair<methodRec*, int>>> implementationMap;


    ParserDriver();
    /**
     * builds all the non letCase environments and symbol tables, so it just traverses the top three levels of the tree
     * TODO: can do this as part of the ast construction, maybe? but it would be bottom up
     */
    void buildEnvs();
    void buildInternalsAst();
    void populateClassImplementationMaps();
    void populateMaps(map<string, set<string>>& graph, string klass); //helper method

    void printClassMap(ostream& os);
    void printImplementationMap(ostream& os);
    void printParentMap(ostream& os);

    int result;

    //main routine is calling the parser
    int parse(const std::string& f);
    int parseBasicClasses(const std::string& source);


        //name of file being parsed
    std::string file;

    bool trace_parsing;

    //handle scanner
    void scan_begin();
    void scan_end();

    void scan_string();

    bool trace_scanning;

    //Token's location used by scanner
    yy::location location;

    //TODO put this in a file
    string basicClassesSource = "--the contents of the methods are ignored, it just has to be parse-able\n"
                                "--order the methods alphabetically so that they print like the ref compiler \n"
                                "--wants them to print\n"
                                "class Object inherits Object {\n"
                                "\tabort() : Object {\n"
                                "\t\tfalse\n"
                                "\t};\n"
                                "\tcopy() : SELF_TYPE {\n"
                                "\t\tfalse\n"
                                "\t};\n"
                                "\ttype_name() : String {\n"
                                "\t\tfalse\n"
                                "\t};\n"
                                "\t\n"
                                "};\n"
                                "\n"
                                "class IO {\n"
                                "\tin_int() : Int {\n"
                                "\t\tfalse\n"
                                "\t};\n"
                                "\tin_string() : String {\n"
                                "\t\tfalse\n"
                                "\t};\n"
                                "\tout_int(x : Int) : SELF_TYPE {\n"
                                "\t\tfalse\n"
                                "\t};\n"
                                "\tout_string(x : String) : SELF_TYPE {\n"
                                "\t\tfalse\n"
                                "\t};\n"
                                "};\n"
                                "\n"
                                "class Int {};\n"
                                "\n"
                                "class String {\n"
                                "\tconcat(s : String) : String {\n"
                                "\t\tfalse\n"
                                "\t};\n"
                                "\tlength() : Int {\n"
                                "\t\tfalse\n"
                                "\t};\n"
                                "\tsubstr(i : Int, l : Int) : String {\n"
                                "\t\tfalse\n"
                                "\t};\n"
                                "};\n"
                                "\n"
                                "class Bool {};";

    void printHi() {cout << "hiiii\n";}
    friend class _node;
private:
};


#endif //PARSERDRIVER_H
