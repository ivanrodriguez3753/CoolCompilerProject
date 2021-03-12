
#include <iostream>
#include <llvm/IR/Verifier.h>
#include "llvm/IR/Module.h"
#include "ParserDriver.hh"
#include "ast.h"


using namespace std;

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
        if(classIt.first == "String") paramTypes.push_back(llvm::Type::getInt8PtrTy(*llvmContext));
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

    //String
    llvm::StructType* llvmType = llvmModule->getTypeByName("String_class_type");
    vector<llvm::Type*> llvmAttributes{
            llvmModule->getTypeByName("String_vtable_type")->getPointerTo(),
            llvmModule->getTypeByName("LLVMString")
    };
    llvmType->setBody(llvmAttributes);


}

void ParserDriver::genClassAndVtableTypeDefs() {
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

void ParserDriver::define_IO_out_string() {
    llvm::Function* IO_out_string_func = llvmModule->getFunction("IO.out_string");
    llvm::Value* self = IO_out_string_func->getArg(0); self->setName("self");
    llvm::Value* printee = IO_out_string_func->getArg(1); printee->setName("printee");
    llvm::BasicBlock* out_string_block = llvm::BasicBlock::Create(*llvmContext, "entry", IO_out_string_func);
    llvmBuilder->SetInsertPoint(out_string_block);
    llvm::Value* castedPrintee = llvmBuilder->CreatePointerCast(
        printee,
        llvmModule->getTypeByName("String_class_type")->getPointerTo(),
        "castedPrintee");
    //essentially using GEP to do a %castedPrintee->1->0 which accesses the LLVMString object, then LLVMString's i8*
    vector<llvm::Value*> indices{
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvmContext), llvm::APInt(32, (uint64_t)1, true)),
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvmContext), llvm::APInt(32, (uint64_t)0, true))
    };
    llvm::Value* raw_LLVMString_ptr = llvmBuilder->CreateStructGEP(castedPrintee, 1, "raw_LLVMString_ptr");
    llvm::Value* char_ptr_ptr = llvmBuilder->CreateStructGEP(raw_LLVMString_ptr, 0, "char_ptr_ptr");
    llvm::Value* char_ptr = llvmBuilder->CreateLoad(char_ptr_ptr, "char_ptr");
    llvmBuilder->CreateCall(llvmModule->getFunction("printf"), vector<llvm::Value*>{char_ptr});
    llvm::Value* castedSelf = llvmBuilder->CreatePointerCast(self, llvm::Type::getInt64PtrTy(*llvmContext), "castedSelf");
    llvmBuilder->CreateRet(castedSelf);
}

void ParserDriver::genBasicClassMethodDefs() {
    addRawFields();
    currentClassEnv = env->getRec("IO")->link;
    currentMethodEnv = ((_class*)(env->getRec("IO")->treeNode))->assemblyConstructorEnv;
    define_IO_ctr();
    currentMethodEnv = currentClassEnv->getMethodRec("out_int")->link;
    define_IO_out_int();
    currentMethodEnv = currentClassEnv->getMethodRec("out_string")->link;
    define_IO_out_string();

    currentClassEnv = env->getRec("String")->link;
    currentMethodEnv = ((_class*)(env->getRec("IO")->treeNode))->assemblyConstructorEnv;
    define_String_ctr();

    currentMethodEnv = nullptr;
    currentClassEnv = nullptr;
}

void ParserDriver::genUserDefinedMethods() {
    for(auto classIt : env->symTable) {
        classRec* rec = (classRec*)classIt.second;
        for(auto methodIt : rec->link->methodsSymTable) {
            if(methodIt.first == "main" && classIt.first == "Main") {
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
    genDeclarations();
    genClassAndVtableTypeDefs();
    gen_llvmStringTypeAndMethods();
    genBasicClassMethodDefs();
    genAssemblyConstructors();
    genUserDefinedMethods();

    genLLVMMain();
}

void ParserDriver::define_IO_ctr() {
    llvm::Function* IO_ctr_func = llvmModule->getFunction("IO..ctr");
    llvm::BasicBlock* ctr_block = llvm::BasicBlock::Create(*llvmContext, "entry", IO_ctr_func);
    llvmBuilder->SetInsertPoint(ctr_block);

    llvmBuilder->CreateRetVoid();
}

void ParserDriver::define_String_ctr() {
    llvm::Function* String_ctr_func = llvmModule->getFunction("String..ctr");
    llvm::Value* self = String_ctr_func->getArg(0); self->setName("self");
    llvm::Value* globalString_ptr = String_ctr_func->getArg(1); globalString_ptr->setName("globalString_ptr");
    llvm::BasicBlock* ctr_block = llvm::BasicBlock::Create(*llvmContext, "entry", String_ctr_func);
    llvmBuilder->SetInsertPoint(ctr_block);
    llvm::Value* LLVMString_ptr = llvmBuilder->CreateStructGEP(llvmModule->getTypeByName("String_class_type"), self, 1, "LLVMString_ptr");
    llvmBuilder->CreateCall(llvmModule->getFunction("LLVMString..ctr"), vector<llvm::Value*>{LLVMString_ptr, globalString_ptr});
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

llvm::Value* _string::codegen(ParserDriver& drv) {
    llvm::Value* str1 = drv.llvmBuilder->CreateAlloca(drv.llvmModule->getTypeByName(type + "_class_type"), 0, nullptr, "str1");
    llvm::Value* globalStrPtr = drv.strLits.at(value).second;
    drv.llvmBuilder->CreateCall(drv.llvmModule->getFunction(type + "..ctr"), vector<llvm::Value*>{str1, globalStrPtr});
    return str1;
}

/**
 * This LLVM String class is taken from
 * https://mapping-high-level-constructs-to-llvm-ir.readthedocs.io/en/latest/appendix-a-how-to-implement-a-string-type-in-llvm/index.html
 */
void ParserDriver::gen_llvmStringTypeAndMethods() {
    llvm::Type* charTy = llvm::Type::getInt8Ty(*llvmContext);
    llvm::PointerType* charPtrTy = llvm::Type::getInt8PtrTy(*llvmContext);
    llvm::Type* int64Ty = llvm::Type::getInt64Ty(*llvmContext);
    llvm::PointerType* int64PtrTy = llvm::Type::getInt64PtrTy(*llvmContext);
    llvm::Type* voidTy = llvm::Type::getVoidTy(*llvmContext);
    llvm::Value* nullTermChar =  llvm::ConstantInt::get(charTy, llvm::APInt(8, (uint64_t)0, true)); //Null terminating character


    //declare malloc
    llvm::FunctionType* malloc_ftype = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(*llvmContext), vector<llvm::Type*>{llvm::Type::getInt64Ty(*llvmContext)}, false);
    llvmModule->getOrInsertFunction("malloc", malloc_ftype);
    //declare free
    llvm::FunctionType* free_ftype = llvm::FunctionType::get(llvm::Type::getVoidTy(*llvmContext), vector<llvm::Type*>{llvm::Type::getInt8PtrTy(*llvmContext)}, false);
    llvmModule->getOrInsertFunction("free", free_ftype);
    //declare memcpy
    llvm::FunctionType* memcpy_ftype = llvm::FunctionType::get(
        charPtrTy,
        vector<llvm::Type*>{charPtrTy, charPtrTy, int64Ty},
        false);
    llvmModule->getOrInsertFunction("memcpy", memcpy_ftype);

    //struct definition
    llvm::StructType* String = llvm::StructType::create(*llvmContext, llvm::StringRef("LLVMString"));
    vector<llvm::Type*> attributes;
    attributes.push_back(llvm::Type::getInt8PtrTy(*llvmContext));
    attributes.push_back(llvm::Type::getInt64Ty(*llvmContext));
    attributes.push_back(llvm::Type::getInt64Ty(*llvmContext));
    attributes.push_back(llvm::Type::getInt64Ty(*llvmContext));
    String->setBody(attributes);

    //declare .concatChar because ..ctr uses it
    vector<llvm::Type*> concatCharParams{String->getPointerTo(), charTy};
    llvm::FunctionType* concatChar_func_type = llvm::FunctionType::get(voidTy, concatCharParams, false);
    llvm::Function* concatChar_func = llvm::Function::Create(concatChar_func_type, llvm::GlobalValue::ExternalLinkage, "LLVMString.concatChar", llvmModule);

    llvm::Argument* this_ptr;
    vector<llvm::Type*> params{String->getPointerTo(), charPtrTy};

    //CONSTRUCTOR
    llvm::FunctionType* ctr_func_type = llvm::FunctionType::get(
        llvm::Type::getVoidTy(*llvmContext),
        params,
        false);
    llvm::Function* ctr_func = llvm::Function::Create(ctr_func_type, llvm::GlobalValue::ExternalLinkage, 0, "LLVMString..ctr", llvmModule);
    this_ptr = ctr_func->getArg(0); this_ptr->setName("self");
    llvm::Argument* globalString_ptr = ctr_func->getArg(1); globalString_ptr->setName("globalString_ptr");
    llvm::BasicBlock* ctr_block = llvm::BasicBlock::Create(*llvmContext, "entry", ctr_func);
    llvm::BasicBlock* loopSetup = llvm::BasicBlock::Create(*llvmContext, "loopSetup", ctr_func);
    llvm::BasicBlock* loopHeader = llvm::BasicBlock::Create(*llvmContext, "loopHeader", ctr_func);
    llvm::BasicBlock* loopBody = llvm::BasicBlock::Create(*llvmContext, "loopBody", ctr_func);
    llvm::BasicBlock* loopEnd = llvm::BasicBlock::Create(*llvmContext, "loopEnd", ctr_func);
    llvmBuilder->SetInsertPoint(ctr_block);
    llvm::Value* char_star_ptr = llvmBuilder->CreateStructGEP(String, this_ptr, 0, "char_ptr_ptr");
    llvmBuilder->CreateStore(llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(*llvmContext)), char_star_ptr);
    llvm::Value* length_ptr = llvmBuilder->CreateStructGEP(String, this_ptr, 1, "length_ptr");
    llvmBuilder->CreateStore(
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(*llvmContext), llvm::APInt(64, (uint64_t)0, true)),
        length_ptr);
    llvm::Value* maxlength_ptr = llvmBuilder->CreateStructGEP(String, this_ptr, 2, "maxlength_ptr");
    llvmBuilder->CreateStore(
        llvm::ConstantInt::get(
            llvm::Type::getInt64Ty(*llvmContext),
            llvm::APInt(64, (uint64_t)0, true)),
        maxlength_ptr);
    llvm::Value* factor_ptr = llvmBuilder->CreateStructGEP(String, this_ptr, 3, "factor_ptr");
    llvmBuilder->CreateStore(
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(*llvmContext), llvm::APInt(64, (uint64_t)16, true)),
        factor_ptr);
    llvmBuilder->CreateBr(loopSetup);

    llvmBuilder->SetInsertPoint(loopSetup);
    llvm::Value* acc_ptr = llvmBuilder->CreateAlloca(int64Ty, nullptr, "acc_ptr");
    llvmBuilder->CreateStore(
        llvm::ConstantInt::get(int64Ty, llvm::APInt(64, (uint64_t)0, true)),
        acc_ptr);
    llvmBuilder->CreateBr(loopHeader);

    llvmBuilder->SetInsertPoint(loopHeader);
    llvm::Value* acc = llvmBuilder->CreateLoad(int64Ty, acc_ptr, "acc");
    llvm::Value* curCharPtr = llvmBuilder->CreateGEP(charTy, globalString_ptr, acc, "curCharPtr");
    llvm::Value* curChar = llvmBuilder->CreateLoad(charTy, curCharPtr, "curChar");
    llvm::Value* nullTermCheck_bool = llvmBuilder->CreateICmp(llvm::CmpInst::ICMP_NE, curChar, nullTermChar, "nullTermCheck_bool");
    llvmBuilder->CreateCondBr(nullTermCheck_bool, loopBody, loopEnd);

    llvmBuilder->SetInsertPoint(loopBody);
    llvmBuilder->CreateCall(llvmModule->getFunction("LLVMString.concatChar"), vector<llvm::Value*>{this_ptr, curChar});
    llvm::Value* accplusone = llvmBuilder->CreateAdd(acc, llvm::ConstantInt::get(int64Ty, 1), "accplusone");
    llvmBuilder->CreateStore(accplusone, acc_ptr);
    llvmBuilder->CreateBr(loopHeader);

    llvmBuilder->SetInsertPoint(loopEnd);
    llvmBuilder->CreateRetVoid();

    //DESTRUCTOR
    params.erase(--(params.end()));
    llvm::FunctionType* dtr_func_type = llvm::FunctionType::get(llvm::Type::getVoidTy(*llvmContext), params, false);
    llvm::Function* dtr_func = llvm::Function::Create(dtr_func_type, llvm::GlobalValue::ExternalLinkage, 0, "LLVMString..dtr", llvmModule);
    this_ptr = dtr_func->getArg(0);  this_ptr->setName("self");//check if need to set name again
    llvm::BasicBlock* dtr_block = llvm::BasicBlock::Create(*llvmContext, "entry", dtr_func);
    llvmBuilder->SetInsertPoint(dtr_block);
    //check if we need to call free
    llvm::Value* char_ptr_ptr = llvmBuilder->CreateStructGEP(String, this_ptr, 0, "char_ptr_ptr");
    llvm::Value* char_ptr = llvmBuilder->CreateLoad(char_ptr_ptr);
    llvm::Value* nullCheck_boolean = llvmBuilder->CreateICmp(llvm::CmpInst::ICMP_NE, char_ptr, llvm::ConstantPointerNull::getNullValue(llvm::Type::getInt8PtrTy(*llvmContext)));
    llvm::BasicBlock* open_block = llvm::BasicBlock::Create(*llvmContext, "free_begin", dtr_func);
    llvm::BasicBlock* close_block = llvm::BasicBlock::Create(*llvmContext, "free_close", dtr_func);
    llvmBuilder->CreateCondBr(nullCheck_boolean, open_block, close_block);
    llvmBuilder->SetInsertPoint(open_block);
    llvmBuilder->CreateCall(llvmModule->getFunction("free"), char_ptr);
    llvmBuilder->CreateBr(close_block);
    llvmBuilder->SetInsertPoint(close_block);
    llvmBuilder->CreateRetVoid();

    //PRINT
    llvm::FunctionType* print_func_type = dtr_func_type;
    llvm::Function* print_func = llvm::Function::Create(print_func_type, llvm::GlobalValue::ExternalLinkage, 0, "LLVMString.print", llvmModule);
    this_ptr = print_func->getArg(0); this_ptr->setName("self");
    llvm::BasicBlock* print_block = llvm::BasicBlock::Create(*llvmContext, "entry", print_func);
    llvmBuilder->SetInsertPoint(print_block);
    char_ptr_ptr = llvmBuilder->CreateStructGEP(String, this_ptr, 0, "char_ptr_ptr");
    char_ptr = llvmBuilder->CreateLoad(char_ptr_ptr, "buffer");
    llvmBuilder->CreateCall(llvmModule->getFunction("printf"), vector<llvm::Value*>{char_ptr});
    llvmBuilder->CreateRetVoid();

    //RESIZE
    params.push_back(int64Ty);
    llvm::FunctionType* resize_func_type = llvm::FunctionType::get(llvm::Type::getVoidTy(*llvmContext), params, false);
    llvm::Function* resize_func = llvm::Function::Create(resize_func_type, llvm::GlobalValue::ExternalLinkage, 0, "LLVMString.resize", llvmModule);
    this_ptr = resize_func->getArg(0); this_ptr->setName("self");
    llvm::Value* r = resize_func->getArg(1); r->setName("value");
    llvm::BasicBlock* resize_block = llvm::BasicBlock::Create(*llvmContext, "entry", resize_func);
    llvmBuilder->SetInsertPoint(resize_block);
    llvm::Value* output = llvmBuilder->CreateCall(llvmModule->getFunction("malloc"), vector<llvm::Value*>{r}, "output");
    //technically we should check the return value but eh
    char_ptr_ptr = llvmBuilder->CreateStructGEP(String, this_ptr, 0, "char_ptr_ptr");
    char_ptr = llvmBuilder->CreateLoad(char_ptr_ptr, "buffer");
    length_ptr = llvmBuilder->CreateStructGEP(String, this_ptr, 1, "length_ptr");
    llvm::Value* length = llvmBuilder->CreateLoad(length_ptr, "length");
    llvmBuilder->CreateCall(llvmModule->getFunction("memcpy"), vector<llvm::Value*>{output, char_ptr, length});
    llvmBuilder->CreateCall(llvmModule->getFunction("free"), vector<llvm::Value*>{char_ptr});
    llvmBuilder->CreateStore(output, char_ptr_ptr);
    llvmBuilder->CreateRetVoid();

    //CONCATCHAR
    this_ptr = concatChar_func->getArg(0); this_ptr->setName("self");
    r = concatChar_func->getArg(1); r->setName("value");
    llvm::BasicBlock* concatChar_block = llvm::BasicBlock::Create(*llvmContext, "entry", concatChar_func);
    llvmBuilder->SetInsertPoint(concatChar_block);
    length_ptr = llvmBuilder->CreateStructGEP(String, this_ptr, 1, "length_ptr");
    length = llvmBuilder->CreateLoad(length_ptr, "length");
    maxlength_ptr = llvmBuilder->CreateStructGEP(String, this_ptr, 2, "maxlength_ptr");
    llvm::Value* maxlength = llvmBuilder->CreateLoad(maxlength_ptr, "maxlength");
    llvm::Value* lengthCheckBool = llvmBuilder->CreateICmp(llvm::CmpInst::ICMP_EQ, length, maxlength);
    llvm::BasicBlock* grow_begin = llvm::BasicBlock::Create(*llvmContext, "grow_begin", concatChar_func);
    llvm::BasicBlock* grow_close = llvm::BasicBlock::Create(*llvmContext, "grow_close", concatChar_func);
    llvmBuilder->CreateCondBr(lengthCheckBool, grow_begin, grow_close);
    llvmBuilder->SetInsertPoint(grow_begin);
    factor_ptr = llvmBuilder->CreateStructGEP(String, this_ptr, 3, "factor_ptr");
    llvm::Value* factor = llvmBuilder->CreateLoad(factor_ptr);
    llvm::Value* sum = llvmBuilder->CreateAdd(maxlength, factor, "sum");
    llvmBuilder->CreateCall(llvmModule->getFunction("LLVMString.resize"), vector<llvm::Value*>{this_ptr, sum});
    llvmBuilder->CreateBr(grow_close);

    llvmBuilder->SetInsertPoint(grow_close);
    char_ptr_ptr = llvmBuilder->CreateStructGEP(String, this_ptr, 0, "char_ptr_ptr");
    char_ptr = llvmBuilder->CreateLoad(char_ptr_ptr, "buffer");
    //append to position length (last index is currently length - 1)
    llvm::Value* bufferAtPosLength_ptr = llvmBuilder->CreateGEP(char_ptr, length);
    llvmBuilder->CreateStore(r, bufferAtPosLength_ptr);
    llvm::Value* lengthplusone = llvmBuilder->CreateAdd(length, llvm::ConstantInt::get(int64Ty, 1), "lengthplusone");
    llvmBuilder->CreateStore(lengthplusone, length_ptr);
    llvmBuilder->CreateRetVoid();

    //CONCAT
    params[1] = params[0]; //we are concatanating a string to self
    llvm::FunctionType* concat_func_type = llvm::FunctionType::get(llvm::Type::getVoidTy(*llvmContext), params, false);
    llvm::Function* concat_func = llvm::Function::Create(concat_func_type, llvm::GlobalValue::ExternalLinkage, "LLVMString.concat", llvmModule);
    this_ptr = concat_func->getArg(0); this_ptr->setName("self");
    llvm::Value* concatThis = concat_func->getArg(1); concatThis->setName("concatThis");
    //this function is a loop that calls concatChar TODO refactor later
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*llvmContext, "entry", concat_func);
    llvm::BasicBlock* loop_header = llvm::BasicBlock::Create(*llvmContext, "loop_header", concat_func);
    llvm::BasicBlock* loop_body = llvm::BasicBlock::Create(*llvmContext, "loop_body", concat_func);
    llvm::BasicBlock* loop_end = llvm::BasicBlock::Create(*llvmContext, "loop_end", concat_func);
    llvmBuilder->SetInsertPoint(entry);
    llvm::Value* i_ptr = llvmBuilder->CreateAlloca(llvm::Type::getInt64Ty(*llvmContext), 0, "i_ptr");
    llvmBuilder->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt64Ty(*llvmContext), 0), i_ptr);
    llvm::Value* concatThisSize_ptr = llvmBuilder->CreateStructGEP(String, concatThis, 1, "concatThis.size_ptr");
    char_ptr_ptr = llvmBuilder->CreateStructGEP(String, concatThis, 0, "char_ptr_ptr");
    char_ptr = llvmBuilder->CreateLoad(char_ptr_ptr, "concatThis.buffer");
    llvmBuilder->CreateBr(loop_header);
    llvmBuilder->SetInsertPoint(loop_header);
    llvm::Value* i = llvmBuilder->CreateLoad(i_ptr);
    llvm::Value* concatThisSize = llvmBuilder->CreateLoad(concatThisSize_ptr);
    llvm::Value* repeat_bool = llvmBuilder->CreateICmp(llvm::CmpInst::ICMP_ULT, i, concatThisSize);
    llvmBuilder->CreateCondBr(repeat_bool, loop_body, loop_end);
    llvmBuilder->SetInsertPoint(loop_body);
    llvm::Value* curChar_ptr = llvmBuilder->CreateGEP(char_ptr, i, "curChar_ptr");
    curChar = llvmBuilder->CreateLoad(curChar_ptr, "curChar");
    llvmBuilder->CreateCall(llvmModule->getFunction("LLVMString.concatChar"), vector<llvm::Value*>{this_ptr, curChar});
    //i++
    llvm::Value* iplusone = llvmBuilder->CreateAdd(i, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*llvmContext), 1), "iplusone");
    llvmBuilder->CreateStore(iplusone, i_ptr);
    llvmBuilder->CreateBr(loop_header);

    llvmBuilder->SetInsertPoint(loop_end);
    llvmBuilder->CreateRetVoid();
}