

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

        //declare struct types (class types and vtable types) and functions
        for(auto it : pdrv.implementationMap) {
            llvm::StructType::create(*llvmContext, llvm::StringRef(it.first + "_class_type"));
            llvm::StructType::create(*llvmContext, llvm::StringRef(it.first + "_vtable_type"));

            for(auto methodIt : pdrv.implementationMap.at(it.first)) {
                ;
            }
        }
        //declare function types
        for(auto it : pdrv.implementationMap)

        //define struct types. first the _class_type's
        for(auto it : pdrv.classMap) {
            llvm::StructType* currentLlvmType = llvmModule->getTypeByName(it.first + "_class_type");

            //Pointer to vtable followed by the real attributes, all of which are pointers to objects
            vector<llvm::Type*> llvmAttributes(pdrv.classMap.at(it.first).size() + 1);
            llvmAttributes.push_back(llvmModule->getTypeByName(it.first + "_vtable_type")->getPointerTo());
            for(auto attrs : pdrv.classMap.at(it.first)) {
                llvmAttributes[attrs.second.second + 1] = llvmModule->getTypeByName(attrs.second.first->type + "_class_type")->getPointerTo();
            }
            currentLlvmType->setBody(llvmAttributes);
        }
        //then the _vtable_type's
        for(auto it : pdrv.implementationMap) {

        }
    }


};
#endif //COOLCOMPILERPROJECTALL_CODEGEN_H
