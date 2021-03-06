

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


    void attempt1() {
        //declare struct types (class types and vtable types)
        for(auto it : pdrv.implementationMap) {
            set<string> doingThese{"IO", "Object", "Main", "String"};
            if(doingThese.find(it.first) == doingThese.end()) {
                continue;
            }
            llvm::StructType::create(*llvmContext, llvm::StringRef(it.first + "_class_type"));
            llvm::StructType::create(*llvmContext, llvm::StringRef(it.first + "_vtable_type"));
        }

        //declare functions
        //has to be in a separate loop from above because a method's parameter types might reference a class that has
        //not been defined yet (in the COOL source code).
        for(auto classIt : pdrv.implementationMap) {
            set<string> doingThese{"IO", "Object", "Main", "String"};
            if(doingThese.find(classIt.first) == doingThese.end()) {
                continue;
            }
            //declare all methods that this class defines
            //these will be global, so the name mangling is <defining class>.<methodIdentifier>
            for (auto methodsIt : pdrv.env->getRec(classIt.first)->link->methodsSymTable) {
                //first, build the function type. Start with parameter types
                vector<llvm::Type *> paramTypes;
                //since all methods in Cool are virtual, we first push back a pointer to self
                paramTypes.push_back(llvmModule->getTypeByName(llvm::StringRef(classIt.first + "_class_type"))->getPointerTo());
                for (int i = 0; i < methodsIt.second->link->symTable.size() - 1; ++i) { //-1 b/c "self" is in symtable, which we accounted for above
                    if(methodsIt.first == "out_string") continue; //TODO delete this
                    //everything in cool is an object, so we need pointers to attributes as attributes
                    //Since we only want one copy of each uniquely defined method, and we have a SELF_TYPE,
                    //we can't always use pointers to llvm types (except Object of course).
                    //todo: find out if non-SELF_TYPE types can be passed a pointer to that class. might avoid some unnecessary
                    // bit casts if that's the case
                    paramTypes.push_back(llvm::PointerType::getInt64PtrTy(*llvmContext));
                }
                //now push back the function with those params, and also an extra pointer for the return value. there are
                //no void expression values in COOL, only an object which is void. so just use another pointer
                llvm::FunctionType *f_type = llvm::FunctionType::get(
                        llvm::PointerType::getInt64PtrTy(*llvmContext),
                        llvm::ArrayRef<llvm::Type*>(paramTypes),
                        false
                );
                llvm::Function::Create(f_type, llvm::GlobalValue::ExternalLinkage, classIt.first + '.' + methodsIt.first, llvmModule);
            }
            vector<llvm::Type*> paramTypes;
            paramTypes.push_back(llvmModule->getTypeByName(llvm::StringRef(classIt.first + "_class_type"))->getPointerTo());
            //no parameters besides self for the constructor
            llvm::FunctionType* f_type = llvm::FunctionType::get(
                    llvm::PointerType::getVoidTy(*llvmContext),
                    llvm::ArrayRef<llvm::Type*>(paramTypes),
                    false
            );
            llvm::Function::Create(f_type, llvm::GlobalValue::ExternalLinkage, classIt.first + '.' + ".ctr", llvmModule); //Class..ctr in case we have a method named ctr (Class.ctr)
        }

        //define _class_types. first the _class_type's
        for(auto it : pdrv.classMap) {
            set<string> doingThese{"IO", "Object", "Main", "String"};
            if(doingThese.find(it.first) == doingThese.end()) {
                continue;
            }
            llvm::StructType* currentLlvmType = llvmModule->getTypeByName(it.first + "_class_type");

            //Pointer to vtable followed by the real attributes, all of which are pointers to objects
            vector<llvm::Type*> llvmAttributes(pdrv.classMap.at(it.first).size() + 1);
            llvmAttributes[0] = llvmModule->getTypeByName(it.first + "_vtable_type")->getPointerTo();
            for(auto attrs : pdrv.classMap.at(it.first)) {
                llvmAttributes[attrs.second.second + 1] = llvm::Type::getInt64PtrTy(*llvmContext);
            }
            currentLlvmType->setBody(llvmAttributes);
        }
        //define_vtable_types
        for(auto it : pdrv.implementationMap) {
            set<string> doingThese{"IO", "Object", "Main", "String"};
            if(doingThese.find(it.first) == doingThese.end()) {
                continue;
            }
            llvm::StructType* currentLlvmStructType = llvmModule->getTypeByName(it.first + "_vtable_type");

            //a list of function pointers
            vector<llvm::Type*> llvmAttributes(it.second.size() + 1);
            //first, a pointer to the constructor
            llvmAttributes[0] = llvmModule->getFunction(it.first + '.' + ".ctr")->getFunctionType()->getPointerTo();
            //all the other methods, placed in the correct spot
            for(auto methodIt : it.second) {
                string definer = methodIt.second.first->link->prev->id;
                llvmAttributes[methodIt.second.second + 1] = llvmModule->getFunction(definer + '.' + methodIt.first)->getFunctionType()->getPointerTo();
            }
            currentLlvmStructType->setBody(llvmAttributes);
        }

    }

    void genMain() {
        llvm::FunctionType* main_ftype = llvm::FunctionType::get(llvm::Type::getInt32Ty(*llvmContext), false);
        llvm::Function* main_func = llvm::Function::Create(main_ftype, llvm::Function::ExternalLinkage, "main", *llvmModule);
        llvm::BasicBlock* main_block = llvm::BasicBlock::Create(*llvmContext, "entry", main_func);
        llvmBuilder->SetInsertPoint(main_block);

        llvm::AllocaInst* IO_instance = llvmBuilder->CreateAlloca(llvmModule->getTypeByName(llvm::StringRef("IO_class_type")), (unsigned)0, nullptr, "io_instance");
        vector<llvm::Value*> params{IO_instance}; //TODO start here: essentially figure out how to get %io_instance to pass it into the call to IO..ctr
//        vector<llvm::Value*> params{llvmBuilder->CreateAlloca(llvmModule->getTypeByName(llvm::StringRef("IO_class_type")), (unsigned)0, nullptr, "io_instance")}; //TODO start here: essentially figure out how to get %io_instance to pass it into the call to IO..ctr
        llvmBuilder->CreateCall(llvmModule->getFunction(llvm::StringRef("IO..ctr")), llvm::ArrayRef<llvm::Value*>(params));
        llvmBuilder->CreateCall(llvmModule->getFunction(llvm::StringRef("IO.out_string")), llvm::ArrayRef<llvm::Value*>(params));


        llvm::APInt returnValue(32, (uint32_t)0, true);
        llvmBuilder->CreateRet(llvm::ConstantInt::get(*llvmContext, returnValue));


        llvm::verifyFunction(*main_func);
    }

    void gen_callprintf() {
        llvm::Function* printf = llvmModule->getFunction("printf");
        vector<llvm::Value*> params{llvmBuilder->CreateGlobalStringPtr(llvm::StringRef("string contents"), ".str.dummyTestString", 0, llvmModule)};
        llvmBuilder->CreateCall(printf, params);
    }

    void define_IO_out_string() {
        llvm::Function* out_string_func = llvmModule->getFunction("IO.out_string");
        llvm::BasicBlock* out_string_block = llvm::BasicBlock::Create(*llvmContext, "entry", out_string_func);
        llvmBuilder->SetInsertPoint(out_string_block);

        gen_callprintf();

        //TODO just return an i64* for now, return the actual correct thing later
        llvm::Value* inttt = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*llvmContext), 123, false);
        llvm::Value* v = llvmBuilder->CreateIntToPtr(inttt, llvm::Type::getInt64PtrTy(*llvmContext, 0));
        llvmBuilder->CreateRet(v);


        llvm::verifyFunction(*out_string_func);
    }

    void define_IO_ctr() {
        llvm::Function* IO_ctr_func = llvmModule->getFunction("IO..ctr");
        llvm::BasicBlock* ctr_block = llvm::BasicBlock::Create(*llvmContext, "entry", IO_ctr_func);
        llvmBuilder->SetInsertPoint(ctr_block);

        llvmBuilder->CreateRetVoid();
    }


    codegenDriver(ParserDriver& d) : pdrv(d) {
        //Open a new context and module
        llvmContext = new llvm::LLVMContext();
        llvmModule = new llvm::Module("theModuleID", *llvmContext);

        //Create a new builder for the module
        llvmBuilder = new llvm::IRBuilder<>(*llvmContext);


        attempt1();
        declareExterns();
        define_IO_ctr();
        define_IO_out_string();
        genMain();

        llvmModule->print(llvm::outs(), nullptr);
    }


    void declareExterns() {
        llvmModule->getOrInsertFunction(
            "printf",
            llvm::FunctionType::get(
                llvm::IntegerType::getInt32Ty(*llvmContext),
                llvm::Type::getInt8Ty(*llvmContext)->getPointerTo(),
                true /* this is var arg func type*/
            )
        );
    }


};
#endif //COOLCOMPILERPROJECTALL_CODEGEN_H
