
#include <iostream>
#include <llvm/IR/Verifier.h>
#include "llvm/IR/Module.h"
#include "ParserDriver.hh"

using namespace std;

void ParserDriver::initializeLLVM() {
    //Open a new context and module
    llvmContext = new llvm::LLVMContext();
    llvmModule = new llvm::Module("theModuleID", *llvmContext);

    //Create a new builder for the module
    llvmBuilder = new llvm::IRBuilder<>(*llvmContext);
}

void ParserDriver::genDeclarations() {
    //declare struct types (class types and vtable types)
    for(auto it : implementationMap) {
        llvm::StructType::create(*llvmContext, llvm::StringRef(it.first + "_class_type"));
        llvm::StructType::create(*llvmContext, llvm::StringRef(it.first + "_vtable_type"));
    }

    //declare functions
    for(auto classIt : implementationMap) {
        //these will be global, so the name mangling is <defining class>.<methodIdentifier>
        for (auto methodsIt : env->getRec(classIt.first)->link->methodsSymTable) {
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
            llvm::Function* f = llvm::Function::Create(f_type, llvm::GlobalValue::ExternalLinkage, classIt.first + '.' + methodsIt.first, llvmModule);
            llvmNamedValues.insert({classIt.first + '.' + methodsIt.first, f});
        }
        //ctr
        vector<llvm::Type*> paramTypes;
        paramTypes.push_back(llvmModule->getTypeByName(llvm::StringRef(classIt.first + "_class_type"))->getPointerTo());
        //no parameters besides self for the constructor
        llvm::FunctionType* f_type = llvm::FunctionType::get(
                llvm::PointerType::getVoidTy(*llvmContext),
                llvm::ArrayRef<llvm::Type*>(paramTypes),
                false
        );
        llvm::Function* f = llvm::Function::Create(f_type, llvm::GlobalValue::ExternalLinkage, classIt.first + '.' + ".ctr", llvmModule); //Class..ctr in case we have a method named ctr (Class.ctr)
        llvmNamedValues.insert({classIt.first + '.' + ".ctr", f});
    }

    //declare extern printf
    llvmModule->getOrInsertFunction(
            "printf",
            llvm::FunctionType::get(
                    llvm::IntegerType::getInt32Ty(*llvmContext),
                    llvm::Type::getInt8Ty(*llvmContext)->getPointerTo(),
                    true /* this is var arg func type*/
            )
    );
}

void ParserDriver::addRawFields() {
    //Bool
    llvm::StructType* llvmBool = llvmModule->getTypeByName("Bool_class_type");
    vector<llvm::Type*> llvmBoolAttributes{
        llvmModule->getTypeByName("Bool_vtable_type")->getPointerTo(),
        llvm::Type::getInt1Ty(*llvmContext)
    };
    llvmBool->setBody(llvmBoolAttributes);

    //Int
    llvm::StructType* llvmInt = llvmModule->getTypeByName("Int_class_type");
    vector<llvm::Type*> llvmIntAttributes{
            llvmModule->getTypeByName("Int_vtable_type")->getPointerTo(),
            llvm::Type::getInt64Ty(*llvmContext)
    };
    llvmInt->setBody(llvmIntAttributes);

    //todo: more string stuff
//    //String
//    llvm::StructType* llvmType = llvmModule->getTypeByName("Bool_class_type");
//    vector<llvm::Type*> llvmAttributes{
//            llvmModule->getTypeByName("Bool_vtable_type")->getPointerTo(),
//            llvm::Type::getInt1Ty(*llvmContext)
//    };
//    llvmType->setBody(llvmAttributes);


}

void ParserDriver::genClassAndVtableTypeDefs() {
    addRawFields();
    //define _class_types
    map<string, map<string, pair<objRec*, int>>> classMapMinusBoolIntString = classMap;
    classMapMinusBoolIntString.erase(classMapMinusBoolIntString.find("Bool"));
    classMapMinusBoolIntString.erase(classMapMinusBoolIntString.find("Int"));
    classMapMinusBoolIntString.erase(classMapMinusBoolIntString.find("String"));
    for(auto it : classMapMinusBoolIntString) {
        llvm::StructType* currentLlvmType = llvmModule->getTypeByName(it.first + "_class_type");

        //Pointer to vtable followed by the real attributes, all of which are pointers to objects
        vector<llvm::Type*> llvmAttributes(classMap.at(it.first).size() + 1);
        llvmAttributes[0] = llvmModule->getTypeByName(it.first + "_vtable_type")->getPointerTo();
        for(auto attrs : classMap.at(it.first)) {
            llvmAttributes[attrs.second.second + 1] = llvm::Type::getInt64PtrTy(*llvmContext);
        }
        currentLlvmType->setBody(llvmAttributes);
    }
    //define_vtable_types
    for(auto it : implementationMap) {
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

void ParserDriver::genBasicClassMethodDefs() {
    currentClassEnv = env->getRec("IO")->link;
    currentMethodEnv = ((_class*)(env->getRec("IO")->treeNode))->assemblyConstructorEnv;
    define_IO_ctr();
    currentMethodEnv = currentClassEnv->getMethodRec("out_int")->link;
    define_IO_out_int();

    currentMethodEnv = nullptr;
    currentClassEnv = nullptr;
}

void ParserDriver::genUserDefinedMethods() {
    for(auto classIt : env->symTable) {
        classRec* rec = (classRec*)classIt.second;
        for(auto methodIt : rec->link->methodsSymTable) {
            if(methodIt.first == "main" && classIt.first == "Main") {
                //set up the LLVM context
                currentClassEnv = env->getRec(classIt.first)->link;
                currentMethodEnv = currentClassEnv->getMethodRec(methodIt.first)->link;
                llvm::Function* func = llvmModule->getFunction(classIt.first + '.' + methodIt.first);
                //name the arguments
                for(auto it : methodIt.second->link->symTable) {
                    objRec* arg = methodIt.second->link->getRec(it.first);
                    func->getArg(arg->localOffset + 1)->setName(it.first);
                }//implicit self arg
                func->getArg(0)->setName("self");

                llvm::BasicBlock* block = llvm::BasicBlock::Create(*llvmContext, "entry", func);
                llvmBuilder->SetInsertPoint(block);

                ((_method*)(methodIt.second->treeNode))->body->codegen(*this);

                //return self, but casted to an i64*
                llvm::Value* castedSelf = llvmBuilder->CreatePointerCast(
                        func->getArg(0),
                        llvm::Type::getInt64PtrTy(*llvmContext),
                        "castedSelf"
                );
                llvmBuilder->CreateRet(castedSelf); //return self
            }
            else {
                ;//do nothing
            }
        }
    }

}

void ParserDriver::genLLVMMain() {
    llvm::FunctionType* main_ftype = llvm::FunctionType::get(llvm::Type::getInt32Ty(*llvmContext), false);
    llvm::Function* main_func = llvm::Function::Create(main_ftype, llvm::Function::ExternalLinkage, "main", *llvmModule);
    llvmNamedValues.insert({"Main.main", main_func});
    llvm::BasicBlock* main_block = llvm::BasicBlock::Create(*llvmContext, "entry", main_func);
    llvmBuilder->SetInsertPoint(main_block);

    llvm::AllocaInst* implicitMain_instance = llvmBuilder->CreateAlloca(llvmModule->getTypeByName(llvm::StringRef("Main_class_type")), (unsigned)0, nullptr, "implicitMain_instance");
    vector<llvm::Value*> params{implicitMain_instance};
    llvmBuilder->CreateCall(llvmModule->getFunction(llvm::StringRef("Main..ctr")), llvm::ArrayRef<llvm::Value*>(params));

    llvmBuilder->CreateCall(llvmModule->getFunction(llvm::StringRef("Main.main")), llvm::ArrayRef<llvm::Value*>(params));

    llvm::APInt returnValue(32, (uint32_t)0, true);
    llvmBuilder->CreateRet(llvm::ConstantInt::get(*llvmContext, returnValue));


    llvm::verifyFunction(*main_func);
}

void ParserDriver::codegen() {
    initializeLLVM();
    genDeclarations();
    genClassAndVtableTypeDefs();
    genBasicClassMethodDefs();
    genAssemblyConstructors();
    genUserDefinedMethods();

    genLLVMMain();

    llvmModule->print(llvm::outs(), nullptr);
}

void ParserDriver::define_IO_ctr() {
    llvm::Function* IO_ctr_func = llvmModule->getFunction("IO..ctr");
    llvm::BasicBlock* ctr_block = llvm::BasicBlock::Create(*llvmContext, "entry", IO_ctr_func);
    llvmBuilder->SetInsertPoint(ctr_block);

    llvmBuilder->CreateRetVoid();
}

void ParserDriver::define_IO_out_int() {
    llvm::Function* out_int_func = llvmModule->getFunction("IO.out_int");
    llvm::BasicBlock* out_int_block = llvm::BasicBlock::Create(*llvmContext, "entry", out_int_func);
    llvmBuilder->SetInsertPoint(out_int_block);
    llvm::Value* castedArg = llvmBuilder->CreatePointerCast(
        out_int_func->getArg(1),
        llvmModule->getTypeByName("Int_class_type")->getPointerTo(),
        "castedArg"
    );

    llvm::Function* printf = llvmModule->getFunction("printf");
    vector<llvm::Value*> printf_args{llvmBuilder->CreateGlobalStringPtr(llvm::StringRef("%d"), ".str.percentd", 0, llvmModule)};
    llvm::Value* raw_int_ptr = llvmBuilder->CreateStructGEP(
        llvmModule->getTypeByName("Int_class_type"),
        castedArg,
        1,
        "raw_int_ptr"
    );
    llvm::Value* raw_int = llvmBuilder->CreateLoad(llvm::Type::getInt64Ty(*llvmContext), raw_int_ptr, "raw_int");
    printf_args.push_back(raw_int);
    llvmBuilder->CreateCall(printf, printf_args);

    //return self, but casted to an i64*
    llvm::Value* castedSelf = llvmBuilder->CreatePointerCast(
        out_int_func->getArg(0),
        llvm::Type::getInt64PtrTy(*llvmContext),
        "castedSelf"
    );
    llvmBuilder->CreateRet(castedSelf); //return self
}

void ParserDriver::gen_callprintf_int() {
    llvm::Function* printf = llvmModule->getFunction("printf");
    vector<llvm::Value*> params{llvmBuilder->CreateGlobalStringPtr(llvm::StringRef("%d"), ".str.percentd", 0, llvmModule)};
    params.push_back(llvm::ConstantInt::get(llvm::Type::getInt64Ty(*llvmContext), 123, true));
    llvmBuilder->CreateCall(printf, params);
}

llvm::Value* _selfDispatch::codegen(ParserDriver &drv) {
    string& definer = drv.implementationMap.at(drv.currentClassEnv->id).at(id).first->definer;

    //this refers to the environment we are in in terms of the tree, NOT in terms of the dispatch
    llvm::Function* funcWeAreDefining = drv.llvmModule->getFunction(drv.currentClassEnv->id + '.' + drv.currentMethodEnv->id);

    llvm::Value* self = funcWeAreDefining->getArg(0);
    //cast to definer type
    self = drv.llvmBuilder->CreatePointerCast(
            self, //source
            drv.llvmModule->getTypeByName(definer + "_class_type")->getPointerTo(), //dest type
            "casted_self"
    );
    vector<llvm::Value*> args{self};

    int argCounter = 1;
    for(auto arg : argList) {
        llvm::Value* uncastedResult = arg->codegen(drv);
        args.push_back(uncastedResult);
    }
    for(int i = 1; i < args.size(); ++i) {
        args[i] = drv.llvmBuilder->CreatePointerCast(
                args[i],
                llvm::Type::getInt64PtrTy(*drv.llvmContext),
                "casted_arg" + to_string(argCounter++)
        );
    }



    return drv.llvmBuilder->CreateCall(
        drv.llvmModule->getFunction(definer + '.' + id),
        args
    );

}

void ParserDriver::genAssemblyConstructors() {
    //TODO: enable support for classes with attributes
    map<string, map<string, pair<objRec*, int>>> classMapMinusBoolIntString = classMap;
    classMapMinusBoolIntString.erase(classMapMinusBoolIntString.find("Bool"));
    classMapMinusBoolIntString.erase(classMapMinusBoolIntString.find("Int"));
    classMapMinusBoolIntString.erase(classMapMinusBoolIntString.find("String"));
    for(auto classIt : classMapMinusBoolIntString) {
        llvm::Function* ctr_func = llvmModule->getFunction(classIt.first + "..ctr");
        llvm::BasicBlock* ctr_block = llvm::BasicBlock::Create(*llvmContext, "entry", ctr_func);
        llvmBuilder->SetInsertPoint(ctr_block);

        llvmBuilder->CreateRetVoid();
    }

    //Bool..ctr

    //Int..ctr
    llvm::Function* int_ctr_func = llvmModule->getFunction("Int..ctr");
    llvm::BasicBlock* int_ctr_block = llvm::BasicBlock::Create(*llvmContext, "entry", int_ctr_func);
    llvmBuilder->SetInsertPoint(int_ctr_block);
    llvm::Argument* this_ptr = int_ctr_func->getArg(0);
    llvm::Value* raw_field_ptr = llvmBuilder->CreateStructGEP(
        llvmModule->getTypeByName("Int_class_type"),
        this_ptr,
        1, //0 is vtable pointer, 1 is first attribute (and in this case only attribute) (the unboxed int)
        "raw_field_ptr"
    );
    llvmBuilder->CreateStore(
        llvm::ConstantInt::get(
            llvm::Type::getInt64Ty(*llvmContext),
            llvm::APInt(64, (uint64_t)0, true)
        ),
        raw_field_ptr
    );
    llvmBuilder->CreateRetVoid();
}

llvm::Value* _int::codegen(ParserDriver& drv) {
    llvm::AllocaInst* Int_instance = drv.llvmBuilder->CreateAlloca(
        drv.llvmModule->getTypeByName("Int_class_type"),
        (unsigned)0,
        nullptr,
        "Int_instance"
    );
    vector<llvm::Value*> params{Int_instance};
    drv.llvmBuilder->CreateCall(drv.llvmModule->getFunction("Int..ctr"), params);

    llvm::Value* raw_field_ptr = drv.llvmBuilder->CreateStructGEP(
        drv.llvmModule->getTypeByName("Int_class_type"),
        Int_instance,
        1, //offset for first attribute
        "raw_field_ptr"
    );
    drv.llvmBuilder->CreateStore(
        llvm::ConstantInt::get(
            llvm::Type::getInt64Ty(*drv.llvmContext),
            llvm::APInt(64, (uint64_t)value, true)
        ),
        raw_field_ptr
    );
    return Int_instance;
}