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
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"



//give Flex the prototype of yylex we want...
#define YY_DECL yy::parser::symbol_type yylex(ParserDriver& drv)
//...and declare it for the parser's sake
YY_DECL;

using namespace std;

class ParserDriver {
public:
    int encountered = 0;

    classEnv* currentClassEnv = nullptr;
    methodEnv* currentMethodEnv = nullptr;

    _program* bisonProduct;

    _program* ast;
    _program* internalsAst;
    globalEnv* env;

    llvm::LLVMContext* llvmContext;
    llvm::Module* llvmModule;
    llvm::IRBuilder<>* llvmBuilder;
    map<string, llvm::Value*> llvmNamedValues;

    /**
     * points to the current letCaseEnv
     */
    letCaseEnv* top = nullptr;
    int letCaseCounter = 0;

    map<string, map<string, pair<objRec*, int>>> classMap;

    /**
     * methodRec* has info about defining class, and int is the position. Don't change the position if the child overrides
     * a parent's method, only change the methodRec*
     */
    map<string, map<string, pair<methodRec*, int>>> implementationMap;

    /**
     * <key, <parent, children> >
     */
    map<string, pair<string, set<string>>> inherGraph;

    ParserDriver();
    /**
     * builds all the non letCase environments and symbol tables, so it just traverses the top three levels of the tree
     * TODO: can do this as part of the ast construction, maybe? but it would be bottom up
     */
    void buildEnvs();
    letCaseEnv* buildLetEnv(_let* letNode);
    vector<letCaseEnv*> buildCaseEnvs(_case* caseNode);
    void buildInternalsAst();
    void populateClassImplementationMaps();
    void populateMaps(string klass); //helper method

    void printClassMap(ostream& os);
    void printImplementationMap(ostream& os);
    void printParentMap(ostream& os);

    void codegen();

    string computeLub(set<string> s);

    int result;

    //main routine is calling the parser
    int parse(const std::string& f);


        //name of file being parsed
    std::string file;

    bool trace_parsing;

    //handle scanner
    void scan_begin();
    void scan_end();

    bool trace_scanning;

    //Token's location used by scanner
    yy::location location;



    friend class _node;
private:
    void initializeLLVM();
    void genDeclarations();
    void genClassAndVtableTypeDefs();
        void addRawFields();
    void genBasicClassMethodDefs();
        void define_IO_ctr();
        void define_IO_out_int();
        void gen_callprintf_int();
    void genAssemblyConstructors();
    void genUserDefinedMethods();
    void genLLVMMain();

};


#endif //PARSERDRIVER_H
