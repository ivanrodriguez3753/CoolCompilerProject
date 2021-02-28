//
// Created by Ivan Rodriguez on 2/28/21.
//


#ifndef COOLCOMPILERPROJECTALL_CODEGEN_H
#define COOLCOMPILERPROJECTALL_CODEGEN_H

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_os_ostream.h"

#include "ParserDriver.hh"


/**
 * very bad design right now but decided there is enough work to merit separating into its own class
 */
class codegenDriver {
public:
    ParserDriver& pdrv;

    llvm::LLVMContext* llvmContext;
    llvm::Module* llvmModule;
    llvm::IRBuilder<>* llvmBuilder;
    map<string, llvm::Value*> llvmNamedValues;


    codegenDriver(ParserDriver& d) : pdrv(d) {
        cout << "inside codegenDriver ctr!\n";
        //Open a new context and module
        llvmContext = new llvm::LLVMContext();
        llvmModule = new llvm::Module("theModuleID", *llvmContext);

        //Create a new builder for the module
        llvmBuilder = new llvm::IRBuilder<>(*llvmContext);

        string result;
        llvm::raw_string_ostream ss(result);
        llvmModule->print(ss, nullptr);
        cout << "Hello world!$$$\n";
        cout << ss.str() << endl;
        cout << "$$$\n";
    }


};

#endif //COOLCOMPILERPROJECTALL_CODEGEN_H
