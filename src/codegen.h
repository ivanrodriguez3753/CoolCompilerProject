

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

    void temporaryReducer() {
        map<string, pair<methodRec*, int>> objectImplementationMap = pdrv.implementationMap.at("Object");
        pdrv.implementationMap.clear();
        pdrv.implementationMap.insert({"Object",objectImplementationMap});

        map<string, pair<objRec*, int>> objectClassMap = pdrv.classMap.at("Object");
        pdrv.classMap.clear();
        pdrv.classMap.insert({"Object", objectClassMap});

        map<string, pair<methodRec*, int>>& mapppp = pdrv.implementationMap.at("Object");
        map<string, methodRec *>& method_mapppp = pdrv.env->getRec("Object")->link->methodsSymTable;
        mapppp.erase(mapppp.find("abort")); method_mapppp.erase(method_mapppp.find("abort"));
        mapppp.erase(mapppp.find("copy")); method_mapppp.erase(method_mapppp.find("copy"));
        mapppp.erase(mapppp.find("type_name")); method_mapppp.erase(method_mapppp.find("type_name"));
        cout << "Hi\n";
    }

    void attempt1() {
        temporaryReducer();
        //declare struct types (class types and vtable types)
        for(auto it : pdrv.implementationMap) {
            llvm::StructType::create(*llvmContext, llvm::StringRef(it.first + "_class_type"));
            llvm::StructType::create(*llvmContext, llvm::StringRef(it.first + "_vtable_type"));
        }

        //declare functions
        //has to be in a separate loop from above because a method's parameter types might reference a class that has
        //not been defined yet (in the COOL source code).
        for(auto classIt : pdrv.implementationMap) {
            //declare all methods that this class defines
            //these will be global, so the name mangling is <defining class>.<methodIdentifier>
            for (auto methodsIt : pdrv.env->getRec(classIt.first)->link->methodsSymTable) {
                //first, build the function type. Start with parameter types
                vector<llvm::Type *> paramTypes;
                //since all methods in Cool are virtual, we first push back a pointer to self
                paramTypes.push_back(llvmModule->getTypeByName(llvm::StringRef(classIt.first + "_class_type"))->getPointerTo());
                for (int i = 0; i < methodsIt.second->link->symTable.size(); ++i) {
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
                    llvm::PointerType::getInt64PtrTy(*llvmContext),
                    llvm::ArrayRef<llvm::Type*>(paramTypes),
                    false
            );
            llvm::Function::Create(f_type, llvm::GlobalValue::ExternalLinkage, classIt.first + '.' + ".ctr", llvmModule); //Class..ctr in case we have a method named ctr (Class.ctr)
        }

        //define _class_types. first the _class_type's
        for(auto it : pdrv.classMap) {
            llvm::StructType* currentLlvmType = llvmModule->getTypeByName(it.first + "_class_type");

            //Pointer to vtable followed by the real attributes, all of which are pointers to objects
            vector<llvm::Type*> llvmAttributes(pdrv.classMap.at(it.first).size() + 1);
            llvmAttributes[0] = llvmModule->getTypeByName(it.first + "_vtable_type")->getPointerTo();
            for(auto attrs : pdrv.classMap.at(it.first)) {
                llvmAttributes[attrs.second.second + 1] = llvmModule->getTypeByName(attrs.second.first->type + "_class_type")->getPointerTo();
            }
            currentLlvmType->setBody(llvmAttributes);
        }
        //define_vtable_types
        for(auto it : pdrv.implementationMap) {
            llvm::StructType* currentLlvmStructType = llvmModule->getTypeByName(it.first + "_vtable_type");

            //a list of function pointers
            vector<llvm::Type*> llvmAttributes(it.second.size() + 1);
            //first, a pointer to the constructor
            llvmAttributes[0] = llvmModule->getFunction(it.first + '.' + ".ctr")->getFunctionType()->getPointerTo();
            //all the other methods, placed in the correct spot
            for(auto methodIt : it.second) {
                string definer = methodIt.second.first->link->prev->id;
                llvmAttributes[methodIt.second.second + 1] = llvmModule->getFunction(definer + '.' + methodIt.first)->getFunctionType();
            }
            currentLlvmStructType->setBody(llvmAttributes);
        }

    }

    void attempt3() {
        //declare struct types (class types and vtable types)
        for(auto it : pdrv.implementationMap) {
            llvm::StructType::create(*llvmContext, llvm::StringRef(it.first + "_class_type"));
            llvm::StructType::create(*llvmContext, llvm::StringRef(it.first + "_vtable_type"));
        }

        //declare functions
        //has to be in a separate loop from above because a method's parameter types might reference a class that has
        //not been defined yet (in the COOL source code).
        for(auto classIt : pdrv.implementationMap) {
            //declare all methods that this class defines
            //these will be global, so the name mangling is <defining class>.<methodIdentifier>
            for (auto methodsIt : pdrv.env->getRec(classIt.first)->link->methodsSymTable) {
                //first, build the function type. Start with parameter types
                vector<llvm::Type *> paramTypes;
                //since all methods in Cool are virtual, we first push back a pointer to self
                paramTypes.push_back(llvm::Type::getInt64Ty(*llvmContext));
//                paramTypes.push_back(llvmModule->getTypeByName(llvm::StringRef(classIt.first + "_class_type"))->getPointerTo());
//                for (int i = 0; i < methodsIt.second->link->symTable.size(); ++i) {
//                    //everything in cool is an object, so we need pointers to attributes as attributes
//                    //Since we only want one copy of each uniquely defined method, and we have a SELF_TYPE,
//                    //we can't always use pointers to llvm types (except Object of course).
//                    //todo: find out if non-SELF_TYPE types can be passed a pointer to that class. might avoid some unnecessary
//                    // bit casts if that's the case
//                    paramTypes.push_back(llvm::PointerType::getInt64PtrTy(*llvmContext));
//                }
                //now push back the function with those params, and also an extra pointer for the return value. there are
                //no void expression values in COOL, only an object which is void. so just use another pointer
                llvm::FunctionType *f_type = llvm::FunctionType::get(
                        llvm::PointerType::getInt64Ty(*llvmContext),
                        llvm::ArrayRef<llvm::Type*>(paramTypes),
                        false
                );
                llvm::Function::Create(f_type, llvm::GlobalValue::ExternalLinkage, classIt.first + '.' + methodsIt.first, llvmModule);
            }
            vector<llvm::Type*> paramTypes;
            paramTypes.push_back(llvmModule->getTypeByName(llvm::StringRef(classIt.first + "_class_type"))->getPointerTo());
            //no parameters besides self for the constructor
            llvm::FunctionType* f_type = llvm::FunctionType::get(
                    llvm::PointerType::getInt64PtrTy(*llvmContext),
                    llvm::ArrayRef<llvm::Type*>(paramTypes),
                    false
            );
            llvm::Function::Create(f_type, llvm::GlobalValue::ExternalLinkage, classIt.first + '.' + ".ctr", llvmModule); //Class..ctr in case we have a method named ctr (Class.ctr)
        }

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
            llvm::StructType* currentLlvmStructType = llvmModule->getTypeByName(it.first + "_vtable_type");

            //a list of function pointers
            vector<llvm::Type*> llvmAttributes(it.second.size() + 1);





            //first, a pointer to the constructor
            llvmAttributes.push_back(llvmModule->getFunction(it.first + '.' + ".ctr")->getFunctionType());

            //all the other methods, placed in the correct spot
            for(auto methodIt : it.second) {
                string definer = methodIt.second.first->link->prev->id;
                llvmAttributes[methodIt.second.second + 1] = llvmModule->getFunction(definer + '.' + methodIt.first)->getFunctionType();
            }
        }

        //now we define the methods
        for(auto classIt : pdrv.implementationMap) {
            for (auto methodsIt : pdrv.env->getRec(classIt.first)->link->methodsSymTable) {
                functionDefiner(methodsIt.second->link->prev->id + '.' + methodsIt.first);
            }
            functionDefiner(classIt.first + "..ctr");
        }
    }

    void functionDefiner(string functionName) {
        using namespace llvm;
        Function* func = llvmModule->getFunction(functionName);
        BasicBlock* entry = BasicBlock::Create(*llvmContext, functionName + "_entry", func);
        llvmBuilder->SetInsertPoint(entry);

        Constant* constt = ConstantInt::get(Type::getInt64Ty(*llvmContext), 122);
        Value* val = ConstantExpr::getIntToPtr(constt, PointerType::getUnqual(Type::getInt64Ty(*llvmContext)));
        llvmBuilder->CreateRet(val);

        cout << "$$$$\n";
        if(verifyFunction(*func/*, &outs())*/)) {
            cerr << "Failed to verify function\n";
            exit(0);
        }
        cout << "abcdef\n";
        func->getReturnType()->print(outs()); cout << "$$$\n";

    }

    void attempt2() {
        using namespace llvm;
        /* Create main function */
        FunctionType *funcType = FunctionType::get(llvmBuilder->getInt32Ty(), false);
        Function *mainFunc = Function::Create(funcType, Function::ExternalLinkage, "main", llvmModule);
        BasicBlock *entry = BasicBlock::Create(*llvmContext, "entrypoint", mainFunc);
        llvmBuilder->SetInsertPoint(entry);

        /* String constant */
        Value *helloWorldStr = llvmBuilder->CreateGlobalStringPtr("hello world!\n");

        /* Create "puts" function */
        std::vector<Type *> putsArgs;
        putsArgs.push_back(llvmBuilder->getInt8Ty()->getPointerTo());
        ArrayRef<Type*> argsRef(putsArgs);
        FunctionType *putsType = FunctionType::get(llvmBuilder->getInt32Ty(), argsRef, false);
        FunctionCallee putsFunc = llvmModule->getOrInsertFunction("puts", putsType);

        /* Invoke it */
        llvmBuilder->CreateCall(putsFunc, helloWorldStr);

        /* Return zero */
        llvmBuilder->CreateRet(ConstantInt::get(*llvmContext, APInt(32, 0)));
    }

    codegenDriver(ParserDriver& d) : pdrv(d) {
        //Open a new context and module
        llvmContext = new llvm::LLVMContext();
        llvmModule = new llvm::Module("theModuleID", *llvmContext);

        //Create a new builder for the module
        llvmBuilder = new llvm::IRBuilder<>(*llvmContext);

//        attempt2();
//        attempt3();
        attempt1();
        llvmModule->print(llvm::outs(), nullptr);
    }


};
#endif //COOLCOMPILERPROJECTALL_CODEGEN_H
