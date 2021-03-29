
#include <iostream>
#include <llvm/IR/Verifier.h>
#include "llvm/IR/Module.h"
#include "ParserDriver.hh"
#include "ast.h"


using namespace std;

const int ctrOffset = 0;
const int firstFuncOffset = 1;

const int vtableOffset = 0;
const int firstAttrOffset = 1;

void ParserDriver::codegen() {
    declareExterns();
    gen_llvmStringTypeAndMethods();
    declaresStructsAndFuncs();
    genVTables();
    genStructDefs();
    genBoolIntStringCtrs();
    genCtrs();
    genBasicClassMethods();
    genUserMethods();
    genLLVMmain();
}

void ParserDriver::declareExterns() {
    //declare extern printf
    llvmModule->getOrInsertFunction(
        "printf",
        llvm::FunctionType::get(
            llvm::IntegerType::getInt32Ty(*llvmContext),
            llvm::Type::getInt8Ty(*llvmContext)->getPointerTo(),
            true));
    //declare extern malloc
    llvm::FunctionType* malloc_ftype = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(*llvmContext), vector<llvm::Type*>{llvm::Type::getInt64Ty(*llvmContext)}, false);
    llvmModule->getOrInsertFunction("malloc", malloc_ftype);

    //declare free
    llvm::FunctionType* free_ftype = llvm::FunctionType::get(llvm::Type::getVoidTy(*llvmContext), vector<llvm::Type*>{llvm::Type::getInt8PtrTy(*llvmContext)}, false);
    llvmModule->getOrInsertFunction("free", free_ftype);

    //declare memcpy
    llvm::Type* charPtrTy = llvm::Type::getInt8PtrTy(*llvmContext);
    llvm::FunctionType* memcpy_ftype = llvm::FunctionType::get(
        charPtrTy, vector<llvm::Type*>{charPtrTy, charPtrTy, llvm::Type::getInt64Ty(*llvmContext)}, false);
    llvmModule->getOrInsertFunction("memcpy", memcpy_ftype);
}

void ParserDriver::gen_llvmStringTypeAndMethods() {
    llvm::Type* charTy = llvm::Type::getInt8Ty(*llvmContext);
    llvm::PointerType* charPtrTy = llvm::Type::getInt8PtrTy(*llvmContext);
    llvm::Type* int64Ty = llvm::Type::getInt64Ty(*llvmContext);
    llvm::Type* voidTy = llvm::Type::getVoidTy(*llvmContext);
    llvm::Value* nullTermChar =  llvm::ConstantInt::get(charTy, llvm::APInt(8, (uint64_t)0, true)); //Null terminating character

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

void ParserDriver::declaresStructsAndFuncs() {
    //declare struct types (class types and vtable types)
    for(auto classIt : implementationMap) {
        llvm::StructType::create(*llvmContext, llvm::StringRef(classIt.first + "_c"));
        llvm::StructType::create(*llvmContext, llvm::StringRef(classIt.first + "_v"));
    }

    //declare user functions and also their assembly constructors
    map<string, map<string, pair<methodRec*, int>>> user_implementationMap = implementationMap;
    user_implementationMap.erase(user_implementationMap.find("Bool"));
    user_implementationMap.erase(user_implementationMap.find("Int"));
    user_implementationMap.erase(user_implementationMap.find("IO"));
    user_implementationMap.erase(user_implementationMap.find("String"));
    user_implementationMap.erase(user_implementationMap.find("Object"));
    for(auto classIt : user_implementationMap) {
        for(auto methodIt : env->getRec(classIt.first)->link->methodsSymTable) {
            vector<llvm::Type*> formalTypes{llvmModule->getTypeByName(classIt.first + "_c")->getPointerTo()};
            map<string, rec*> symTableNoSelf = methodIt.second->link->symTable;
            symTableNoSelf.erase(symTableNoSelf.find("self"));
            for(auto sym : symTableNoSelf) {
                const string& type = ((objRec*)sym.second)->type;
                if(type != "SELF_TYPE") {
                    formalTypes.push_back(llvmModule->getTypeByName(type + "_c")->getPointerTo());
                }
                else {
                    formalTypes.push_back(llvm::Type::getInt64PtrTy(*llvmContext));
                }
            }
            llvm::FunctionType* f_type;
            const string& retType = methodIt.second->returnType;
            if(retType != "SELF_TYPE") {
                f_type = llvm::FunctionType::get(
                    llvmModule->getTypeByName(retType + "_c")->getPointerTo(), formalTypes, false);
            }
            else {
                f_type = llvm::FunctionType::get(
                    llvm::Type::getInt64PtrTy(*llvmContext), formalTypes, false);
            }
            llvm::Function::Create(f_type, llvm::GlobalValue::ExternalLinkage, classIt.first + '.' + methodIt.first, llvmModule);
        }

        //constructor
        vector<llvm::Type*> formalTypes{llvmModule->getTypeByName(classIt.first + "_c")->getPointerTo()};
        llvm::FunctionType* f_type = llvm::FunctionType::get(
            llvm::Type::getVoidTy(*llvmContext), formalTypes, false);
        llvm::Function::Create(f_type, llvm::GlobalValue::ExternalLinkage, 0, classIt.first + '.' + ".ctr", llvmModule);
    }

    //declare basic class functions
    map<string, map<string, pair<methodRec*, int>>> basic_implementationMap{
        {"Bool", implementationMap.at("Bool")},
        {"Int", implementationMap.at("Int")},
        {"IO", implementationMap.at("IO")},
        {"String", implementationMap.at("String")},
        {"Object", implementationMap.at("Object")}
    };
    for(auto classIt : basic_implementationMap) {
        for(auto methodIt : env->getRec(classIt.first)->link->methodsSymTable) {
            vector<llvm::Type*> formalTypes{llvmModule->getTypeByName(classIt.first + "_c")->getPointerTo()};
            map<string, rec*> symTableNoSelf = methodIt.second->link->symTable;
            symTableNoSelf.erase(symTableNoSelf.find("self"));
            for(auto sym : symTableNoSelf) {
                const string& type = ((objRec*)sym.second)->type;
                if(type != "SELF_TYPE") {
                    formalTypes.push_back(llvmModule->getTypeByName(type + "_c")->getPointerTo());
                }
                else {
                    formalTypes.push_back(llvm::PointerType::getInt64PtrTy(*llvmContext));
                }
            }
            llvm::FunctionType* f_type;
            const string& retType = methodIt.second->returnType;
            if(retType != "SELF_TYPE") {
                f_type = llvm::FunctionType::get(
                    llvmModule->getTypeByName(retType + "_c")->getPointerTo(), formalTypes, false);
            }
            else {
                f_type = llvm::FunctionType::get(llvm::Type::getInt64PtrTy(*llvmContext), formalTypes, false);
            }
            llvm::Function::Create(f_type, llvm::GlobalValue::ExternalLinkage, classIt.first + '.' + methodIt.first, llvmModule);
        }
    }

    //declare basic class constructors
    vector<llvm::Type*> formalTypes; llvm::FunctionType* f_type; llvm::Type* voidTy = llvm::Type::getVoidTy(*llvmContext);
    //Bool
    formalTypes.push_back(llvmModule->getTypeByName("Bool_c")->getPointerTo());
    formalTypes.push_back(llvm::Type::getInt1Ty(*llvmContext));
    f_type = llvm::FunctionType::get(voidTy, formalTypes, false);
    llvm::Function::Create(f_type, llvm::GlobalValue::ExternalLinkage, 0, "Bool..ctr", llvmModule);
    //Int
    formalTypes.clear();
    formalTypes.push_back(llvmModule->getTypeByName("Int_c")->getPointerTo());
    formalTypes.push_back(llvm::Type::getInt64Ty(*llvmContext));
    f_type = llvm::FunctionType::get(voidTy, formalTypes, false);
    llvm::Function::Create(f_type, llvm::GlobalValue::ExternalLinkage, 0, "Int..ctr", llvmModule);
    //IO
    formalTypes.clear();
    formalTypes.push_back(llvmModule->getTypeByName("IO_c")->getPointerTo());
    f_type = llvm::FunctionType::get(voidTy, formalTypes, false);
    llvm::Function::Create(f_type, llvm::GlobalValue::ExternalLinkage, 0, "IO..ctr", llvmModule);
    //String
    formalTypes.clear();
    formalTypes.push_back(llvmModule->getTypeByName("String_c")->getPointerTo());
    formalTypes.push_back(llvmModule->getTypeByName("LLVMString"));
    f_type = llvm::FunctionType::get(voidTy, formalTypes, false);
    llvm::Function::Create(f_type, llvm::GlobalValue::ExternalLinkage, 0, "String..ctr", llvmModule);
    //Object
    formalTypes.clear();
    formalTypes.push_back(llvmModule->getTypeByName("Object_c")->getPointerTo());
    f_type = llvm::FunctionType::get(voidTy, formalTypes, false);
    llvm::Function::Create(f_type, llvm::GlobalValue::ExternalLinkage, 0, "Object..ctr", llvmModule);
}

void ParserDriver::genVTables() {
    llvm::FunctionType* voidVarArg_type = llvm::FunctionType::get(llvm::Type::getVoidTy(*llvmContext), true);
    for(auto classIt : implementationMap) {
        vector<llvm::Constant*> funcPtrs(1 + classIt.second.size());
        //push the assembly constructor, then methods
        llvm::Function* f = llvmModule->getFunction(classIt.first + "..ctr");
        llvm::Value* bitcastedFunc = llvmBuilder->CreateBitCast(f, voidVarArg_type->getPointerTo());
        funcPtrs[0] = (llvm::Constant*)bitcastedFunc;
        for(auto methodIt : implementationMap.at(classIt.first)) {
            llvm::Function* f = llvmModule->getFunction(methodIt.second.first->definer + '.' + methodIt.first);
            llvm::Value* bitcastedFunc = llvmBuilder->CreateBitCast(f, voidVarArg_type->getPointerTo());
            funcPtrs[1 + methodIt.second.second] = ((llvm::Constant*)bitcastedFunc);
        }
        llvm::ArrayType* table_type = llvm::ArrayType::get(voidVarArg_type->getPointerTo(), classIt.second.size() + 1);
        llvm::GlobalVariable* global = (llvm::GlobalVariable*)llvmModule->getOrInsertGlobal(classIt.first + "_v", table_type);
        global->setInitializer(llvm::ConstantArray::get(table_type, funcPtrs));
        global->setConstant(true);
    }
}
void ParserDriver::genStructDefs() {
    //All structures are laid out as follows: vtable ptr, then all attributes
    //Bool/Int/String are handled separately because their struct defs differ from what they have on the symbol table
    map<string, map<string, pair<objRec*, int>>> classMap_noPrimitives = classMap;
    classMap_noPrimitives.erase(classMap_noPrimitives.find("Bool"));
    classMap_noPrimitives.erase(classMap_noPrimitives.find("Int"));
    classMap_noPrimitives.erase(classMap_noPrimitives.find("String"));
    for(auto classIt : classMap_noPrimitives) {
        vector<llvm::Type*> attrs(classIt.second.size() + firstAttrOffset);
        attrs[0] = llvm::FunctionType::get(llvm::Type::getVoidTy(*llvmContext), true)->getPointerTo()->getPointerTo();
        for(auto attrIt : classIt.second) {
            const string& type = attrIt.second.first->type;
            if(type != "SELF_TYPE") {
                attrs[firstAttrOffset + attrIt.second.second] = llvmModule->getTypeByName(type + "_c")->getPointerTo();
            }
            else {
                attrs[firstAttrOffset + attrIt.second.second] = llvm::Type::getInt64PtrTy(*llvmContext);
            }
        }
        llvmModule->getTypeByName(classIt.first + "_c")->setBody(attrs);
    }
    //Now for special cases of Bool/Int/String (raw fields)
    vector<llvm::Type*> attrs(2);
    attrs[0] = llvm::FunctionType::get(llvm::Type::getVoidTy(*llvmContext), true)->getPointerTo()->getPointerTo();
    //Bool
    attrs[firstAttrOffset] = llvm::Type::getInt1Ty(*llvmContext);
    llvmModule->getTypeByName("Bool_c")->setBody(attrs);
    //Int
    attrs[firstAttrOffset] = llvm::Type::getInt64Ty(*llvmContext);
    llvmModule->getTypeByName("Int_c")->setBody(attrs);
    //String
    attrs[firstAttrOffset] = llvmModule->getTypeByName("LLVMString");
    llvmModule->getTypeByName("String_c")->setBody(attrs);
}

void ParserDriver::genBoolIntStringCtrs() {
    llvm::Function* f; llvm::BasicBlock* b; llvm::Argument* self; llvm::Argument* raw;
    llvm::Value *raw_ptr, *selfPtr_ptr, *loadedSelf, *vtablePtr;

    //Bool
    f = llvmModule->getFunction("Bool..ctr"); self = f->getArg(0); raw = f->getArg(1);
    self->setName("self"); raw->setName("rawBool");
    b = llvm::BasicBlock::Create(*llvmContext, "entry", f);
    llvmBuilder->SetInsertPoint(b);
    vtablePtr = llvmBuilder->CreateStructGEP(self, 0, "vtablePtr");
    llvmBuilder->CreateStore(llvmBuilder->CreateStructGEP(llvmModule->getNamedGlobal("Bool_v"), 0), vtablePtr);
    raw_ptr = llvmBuilder->CreateStructGEP(llvmModule->getTypeByName("Bool_c"), self, firstAttrOffset, "raw_ptr");
    llvmBuilder->CreateStore(raw, raw_ptr);
    llvmBuilder->CreateRetVoid();

    //Int
    f = llvmModule->getFunction("Int..ctr"); self = f->getArg(0); raw = f->getArg(1);
    self->setName("self"); raw->setName("rawInt");
    b = llvm::BasicBlock::Create(*llvmContext, "entry", f);
    llvmBuilder->SetInsertPoint(b);
    vtablePtr = llvmBuilder->CreateStructGEP(self, 0, "vtablePtr");
    llvmBuilder->CreateStore(llvmBuilder->CreateStructGEP(llvmModule->getNamedGlobal("Int_v"), 0), vtablePtr);
    raw_ptr = llvmBuilder->CreateStructGEP(llvmModule->getTypeByName("Int_c"), self, firstAttrOffset, "raw_ptr");
    llvmBuilder->CreateStore(raw, raw_ptr);
    llvmBuilder->CreateRetVoid();

    //String
    f = llvmModule->getFunction("String..ctr"); self = f->getArg(0); raw = f->getArg(1);
    self->setName("self"); raw->setName("rawLLVMString");
    b = llvm::BasicBlock::Create(*llvmContext, "entry", f);
    llvmBuilder->SetInsertPoint(b);
    vtablePtr = llvmBuilder->CreateStructGEP(self, 0, "vtablePtr");
    llvmBuilder->CreateStore(llvmBuilder->CreateStructGEP(llvmModule->getNamedGlobal("String_v"), 0), vtablePtr);
    raw_ptr = llvmBuilder->CreateStructGEP(llvmModule->getTypeByName("String_c"), self, firstAttrOffset, "raw_ptr");
    llvmBuilder->CreateStore(raw, raw_ptr);
    llvmBuilder->CreateRetVoid();
}
void ParserDriver::genCtrs() {
    for(auto classIt : classMap) {
        llvm::Function* f = llvmModule->getFunction(classIt.first + "..ctr");

        cur_func = f; currentClassEnv = env->getRec(classIt.first)->link;
        currentMethodEnv = ((_class*)(env->getRec(classIt.first)->treeNode))->assemblyConstructorEnv;

        llvm::Value* self = f->getArg(0); self->setName("self");
        llvm::BasicBlock* entry_b = llvm::BasicBlock::Create(*llvmContext, "entry", f);
        llvmBuilder->SetInsertPoint(entry_b);
        //Do the vtable
        llvm::Value* vtablePtr = llvmBuilder->CreateStructGEP(self, 0, "vtablePtr");
        llvmBuilder->CreateStore(
            llvmBuilder->CreateStructGEP(llvmModule->getNamedGlobal(classIt.first + "_v"), 0),
            vtablePtr);
        //make a block for each attribute and place in correct order so that attributes get initialized top to bottom, as
        //the user would expect
        vector<llvm::BasicBlock*> attrBlocks(classIt.second.size());
        for(auto attrIt : classIt.second) {
            attrBlocks[attrIt.second.second] = llvm::BasicBlock::Create(*llvmContext, attrIt.first + ".attr.init", cur_func);
        }
        for(int i = 0; i < (int)attrBlocks.size() - 1; ++i) {
            attrBlocks[i]->moveBefore(attrBlocks[i + 1]);
        }


        for(auto attrIt : classIt.second) {
            llvm::Type* llvmType;
            const string& type = attrIt.second.first->type;
            string resolvedType = type;
            if(type == "SELF_TYPE") {
                resolvedType = classIt.first;
                llvmType = llvmModule->getTypeByName(classIt.first + "_c")->getPointerTo();
            }
            else {
                llvmType = llvmModule->getTypeByName(type + "_c")->getPointerTo();
            }
            llvm::BasicBlock* attr_b = attrBlocks[attrIt.second.second];
            llvmBuilder->SetInsertPoint(attr_b);

            llvm::Value* storeAtEnd;
            _expr*& optInit = ((_attr*)(attrIt.second.first->treeNode))->optInit;
            if(resolvedType == "String" || resolvedType == "Int" || resolvedType == "Bool") {
                if(resolvedType == "String") {
                    if(!optInit) {
                        optInit = new _string(0, ""); //random line number which will never be used, and default "" value
                    }
                    storeAtEnd = optInit->codegen(*this);
                }
                else if(resolvedType == "Int") {
                    if(!optInit) {
                        optInit = new _int(0, 0); //random line number which will never be used, and default 0 value
                    }
                    storeAtEnd = optInit->codegen(*this);
                }
                else if(resolvedType == "Bool") {
                    if(!optInit) {
                        optInit = new _bool(0, false); //random line number which will never be used, and default false value
                    }
                    storeAtEnd = optInit->codegen(*this);
                }
            }
            else {
                if(optInit) {
                    storeAtEnd = optInit->codegen(*this);
                }
                else {
                    storeAtEnd = llvm::Constant::getNullValue(llvmType);
                }
            }
            llvmBuilder->CreateStore(
                storeAtEnd,
                llvmBuilder->CreateStructGEP(self, firstAttrOffset + attrIt.second.second));
        }

        llvm::BasicBlock* end_b = llvm::BasicBlock::Create(*llvmContext, "end", cur_func);
        llvmBuilder->SetInsertPoint(end_b);
        llvmBuilder->CreateRetVoid();

        //now take care of LLVM's required br instruction on block dropthroughs
        llvmBuilder->SetInsertPoint(entry_b);
        if(attrBlocks.size()) {
            llvmBuilder->CreateBr(attrBlocks[0]);
            llvmBuilder->SetInsertPoint(attrBlocks[0]);
            for(int i = 1; i < (int)attrBlocks.size() ; ++i) {
                llvmBuilder->CreateBr(attrBlocks[i]);
                llvmBuilder->SetInsertPoint(attrBlocks[i]);
            }
            llvmBuilder->SetInsertPoint(attrBlocks[(int)attrBlocks.size() - 1]);
        }
        llvmBuilder->CreateBr(end_b);
    }
}
void ParserDriver::genBasicClassMethods() {
    genIO_out_int();
    genIO_out_string();
    genIO_in_int();
    genIO_in_string();
    genObject_abort();
    genObject_copy();
    genObject_type_name();
    genString_concat();
    genString_length();
    genString_substr();
}

void ParserDriver::genIO_out_int() {
    llvm::Function* out_int_func = llvmModule->getFunction("IO.out_int");
    llvm::BasicBlock* out_int_block = llvm::BasicBlock::Create(*llvmContext, "entry", out_int_func);
    llvmBuilder->SetInsertPoint(out_int_block);
    llvm::Value* castedArg = llvmBuilder->CreatePointerCast(
        out_int_func->getArg(1),
        llvmModule->getTypeByName("Int_c")->getPointerTo(),
        "castedArg"
    );

    llvm::Function* printf = llvmModule->getFunction("printf");
    vector<llvm::Value*> printf_args{llvmBuilder->CreateGlobalStringPtr(llvm::StringRef("%d"), ".str.percentd", 0, llvmModule)};
    llvm::Value* raw_int_ptr = llvmBuilder->CreateStructGEP(
        llvmModule->getTypeByName("Int_c"),
        castedArg,
        firstAttrOffset,
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
void ParserDriver::genIO_out_string() {
    llvm::Function* f = llvmModule->getFunction("IO.out_string");
    llvm::BasicBlock* b = llvm::BasicBlock::Create(*llvmContext, "entry", f);
    llvm::Value* self = f->getArg(0); self->setName("self");
    llvm::Value* x = f->getArg(1); x->setName("x");
    llvmBuilder->SetInsertPoint(b);
    llvm::Value* raw_LLVMString_ptr = llvmBuilder->CreateStructGEP(x, 1, "raw_LLVMString_ptr");
    llvm::Value* charPtr_ptr = llvmBuilder->CreateStructGEP(raw_LLVMString_ptr, 0, "charPtr_ptr");
    llvm::Value* charPtr = llvmBuilder->CreateLoad(charPtr_ptr, "charPtr");
    llvmBuilder->CreateCall(llvmModule->getFunction("printf"), charPtr);
    llvm::Value* castedSelf = llvmBuilder->CreateBitCast(self, llvm::Type::getInt64PtrTy(*llvmContext), "castedSelf");
    llvmBuilder->CreateRet(castedSelf);


}
void ParserDriver::genIO_in_int() {
    llvm::Function* f = llvmModule->getFunction("IO.in_int");
    llvm::BasicBlock* b = llvm::BasicBlock::Create(*llvmContext, "entry", f);
    llvmBuilder->SetInsertPoint(b);
    //TODO implement, returning null for now so it compiles in LLVM
    llvm::Value* retThis = llvmBuilder->CreateAlloca(llvmModule->getTypeByName("Int_c"));
    llvmBuilder->CreateRet(retThis);
}
void ParserDriver::genIO_in_string() {
    llvm::Function* f = llvmModule->getFunction("IO.in_string");
    llvm::BasicBlock* b = llvm::BasicBlock::Create(*llvmContext, "entry", f);
    llvmBuilder->SetInsertPoint(b);
    //TODO implement, returning null for now so it compiles in LLVM
    llvm::Value* retThis = llvmBuilder->CreateAlloca(llvmModule->getTypeByName("String_c"));
    llvmBuilder->CreateRet(retThis);
}
void ParserDriver::genObject_abort() {
    llvm::Function* f = llvmModule->getFunction("Object.abort");
    llvm::BasicBlock* b = llvm::BasicBlock::Create(*llvmContext, "entry", f);
    llvmBuilder->SetInsertPoint(b);
    //TODO implement, returning null for now so it compiles in LLVM
    llvm::Value* retThis = llvmBuilder->CreateAlloca(llvmModule->getTypeByName("Object_c"));
    llvmBuilder->CreateRet(retThis);
}
void ParserDriver::genObject_copy() {
    llvm::Function* f = llvmModule->getFunction("Object.copy");
    llvm::BasicBlock* b = llvm::BasicBlock::Create(*llvmContext, "entry", f);
    llvmBuilder->SetInsertPoint(b);
    //TODO implement, returning null for now so it compiles in LLVM
    llvm::Value* retThis = llvmBuilder->CreateAlloca(llvm::Type::getInt64Ty(*llvmContext));
    llvmBuilder->CreateRet(retThis);

}
void ParserDriver::genObject_type_name() {
    llvm::Function* f = llvmModule->getFunction("Object.type_name");
    llvm::BasicBlock* b = llvm::BasicBlock::Create(*llvmContext, "entry", f);
    llvmBuilder->SetInsertPoint(b);
    //TODO implement, returning null for now so it compiles in LLVM
    llvm::Value* retThis = llvmBuilder->CreateAlloca(llvmModule->getTypeByName("String_c"));
    llvmBuilder->CreateRet(retThis);
}
void ParserDriver::genString_concat() {
    llvm::Function* f = llvmModule->getFunction("String.concat");
    llvm::BasicBlock* b = llvm::BasicBlock::Create(*llvmContext, "entry", f);
    llvmBuilder->SetInsertPoint(b);
    //TODO implement, returning null for now so it compiles in LLVM
    llvm::Value* retThis = llvmBuilder->CreateAlloca(llvmModule->getTypeByName("String_c"));
    llvmBuilder->CreateRet(retThis);
}
void ParserDriver::genString_length() {
    llvm::Function* f = llvmModule->getFunction("String.length");
    llvm::BasicBlock* b = llvm::BasicBlock::Create(*llvmContext, "entry", f);
    llvmBuilder->SetInsertPoint(b);
    //TODO implement, returning null for now so it compiles in LLVM
    llvm::Value* retThis = llvmBuilder->CreateAlloca(llvmModule->getTypeByName("Int_c"));
    llvmBuilder->CreateRet(retThis);
}
void ParserDriver::genString_substr() {
    llvm::Function* f = llvmModule->getFunction("String.substr");
    llvm::BasicBlock* b = llvm::BasicBlock::Create(*llvmContext, "entry", f);
    llvmBuilder->SetInsertPoint(b);
    //TODO implement, returning null for now so it compiles in LLVM
    llvm::Value* retThis = llvmBuilder->CreateAlloca(llvmModule->getTypeByName("String_c"));
    llvmBuilder->CreateRet(retThis);
}

void ParserDriver::genUserMethods() {
    map<string, rec*> user_symTable = env->symTable;
    user_symTable.erase(user_symTable.find("Bool"));
    user_symTable.erase(user_symTable.find("Int"));
    user_symTable.erase(user_symTable.find("String"));
    user_symTable.erase(user_symTable.find("IO"));
    user_symTable.erase(user_symTable.find("Object"));

    for(auto classIt : user_symTable) {
        classRec* klassRec = (classRec*)classIt.second;
        currentClassEnv = klassRec->link;
        for(auto methodIt : currentClassEnv->methodsSymTable) {
            cur_func = llvmModule->getFunction(classIt.first + '.' + methodIt.first);
            currentMethodEnv = methodIt.second->link;
            llvm::BasicBlock* b = llvm::BasicBlock::Create(*llvmContext, "entry", cur_func);
            llvmBuilder->SetInsertPoint(b);
            _method* methodNode = (_method*)methodIt.second->treeNode;

            //formals cannot have SELF_TYPE so don't need to check

            for(int i = 0; i < methodNode->formalsList.size(); ++i) {
                _formal*& formal = methodNode->formalsList[i];
                const string formalKey = "methodParam." + formal->id;
                currentMethodEnv->localsMap.insert({
                    formalKey,
                    llvmBuilder->CreateAlloca(llvmModule->getTypeByName(formal->type + "_c")->getPointerTo())});
                llvmBuilder->CreateStore(cur_func->getArg(1 + i), currentMethodEnv->localsMap[formalKey]); //first arg is always self so offset
            }
            llvm::Value* ret = methodNode->body->codegen(*this);

            string resolvedType = methodIt.second->returnType;
            if(methodIt.second->returnType == "SELF_TYPE") resolvedType = classIt.first;
            llvm::Value* ret_cast = llvmBuilder->CreateBitCast(
                ret,
                llvmModule->getTypeByName(resolvedType + "_c")->getPointerTo(),
                "ret_cast");
            llvmBuilder->CreateRet(ret_cast);
        }
    }
}

void ParserDriver::genLLVMmain() {
    llvm::FunctionType* f_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(*llvmContext), false);
    llvm::Function* LLVMmain = llvm::Function::Create(f_type, llvm::GlobalValue::ExternalLinkage, "main", *llvmModule);
    llvm::BasicBlock* b = llvm::BasicBlock::Create(*llvmContext, "entry", LLVMmain);
    llvmBuilder->SetInsertPoint(b);
    llvm::Value* MainPtr_ptr = llvmBuilder->CreateAlloca(llvmModule->getTypeByName("Main_c")->getPointerTo(), nullptr, "MainPtr_ptr");
    llvm::Value* mallocRes = llvmBuilder->CreateCall(
        llvmModule->getFunction("malloc"),
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(*llvmContext), llvm::APInt(64, 8, false)));
    llvm::Value* castedMallocRes = llvmBuilder->CreateBitCast(mallocRes, llvmModule->getTypeByName("Main_c")->getPointerTo());
    llvmBuilder->CreateStore(castedMallocRes, MainPtr_ptr);
    llvm::Value* loadedMain = llvmBuilder->CreateLoad(MainPtr_ptr);
    llvmBuilder->CreateCall(llvmModule->getFunction("Main..ctr"), loadedMain);
    llvmBuilder->CreateCall(llvmModule->getFunction("Main.main"), loadedMain);
    llvmBuilder->CreateRet(llvm::ConstantInt::get(llvm::Type::getInt64Ty(*llvmContext), llvm::APInt(32, 0, false)));
}



llvm::Value* _selfDispatch::codegen(ParserDriver& drv) {
    llvm::Value* self = drv.cur_func->getArg(0); self->setName("self");
    const string& definer = drv.implementationMap.at(drv.currentClassEnv->id).at(id).first->definer;
    llvm::Value* selfCast = drv.llvmBuilder->CreateBitCast(self, drv.llvmModule->getTypeByName(definer + "_c")->getPointerTo(), "selfCast");
    vector<llvm::Value*> args{selfCast};
    for(auto arg : argList) {
        args.push_back(arg->codegen(drv));
    }

    llvm::Value* vtablePtr_ptr = drv.llvmBuilder->CreateStructGEP(self, vtableOffset, "vtablePtr_ptr");
    llvm::Value* vtablePtr = drv.llvmBuilder->CreateLoad(vtablePtr_ptr);
    int funcOffset = drv.implementationMap.at(drv.currentClassEnv->id).at(id).second;
    llvm::Value* funcPtr_ptr = drv.llvmBuilder->CreateGEP(
        vtablePtr,
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(*drv.llvmContext), llvm::APInt(64, firstFuncOffset + funcOffset, false)),
        "funcPtr_ptr");
    llvm::Value* funcPtr = drv.llvmBuilder->CreateLoad(funcPtr_ptr, "funcPtr");
    llvm::Value* func = drv.llvmBuilder->CreateBitCast(funcPtr, drv.llvmModule->getFunction(definer + '.' + id)->getType(), id);
    llvm::FunctionCallee fc((llvm::FunctionType*)drv.llvmModule->getFunction(definer + '.' + id)->getType(), func);

    llvm::CallInst* ret = drv.llvmBuilder->CreateCall(fc, args, "ret");
    //TODO figure out why ret type is a function pointer. for now, use mutateType
    ret->mutateType(drv.llvmModule->getFunction(definer + '.' + id)->getReturnType());

    return ret;
}

llvm::Value* _dynamicDispatch::codegen(ParserDriver& drv) {
    vector<llvm::Value*> args{nullptr};//will replace with self after checking if null and casting
    for(_expr* arg : argList) {
        args.push_back(arg->codegen(drv));
    }

    //codegen the caller
    args[0] = caller->codegen(drv);
    llvm::BasicBlock* isNull_b = llvm::BasicBlock::Create(*drv.llvmContext, "isNull", drv.cur_func);
    llvm::BasicBlock* notNull_b = llvm::BasicBlock::Create(*drv.llvmContext, "notNull", drv.cur_func);
    llvm::Value* isNull = drv.llvmBuilder->CreateICmp(llvm::CmpInst::ICMP_EQ, args[0], llvm::Constant::getNullValue(args[0]->getType()));
    drv.llvmBuilder->CreateCondBr(isNull, isNull_b, notNull_b);

    drv.llvmBuilder->SetInsertPoint(isNull_b);
    drv.llvmBuilder->CreateBr(notNull_b);

    drv.llvmBuilder->SetInsertPoint(notNull_b);

    //we're only interested in the INDEX of the function we're trying to call, and the function index will be the same in any function
    //that is part of the same hierarchy, so just use the static type of the callerExpr to get A methodRec that will do
    //NOTE: This is not necessarily the correct methodRec, but its offset info and method signature are sufficient
    string resolvedType = caller->type;
    if(resolvedType == "SELF_TYPE") resolvedType = drv.currentClassEnv->id;

    methodRec* rec = drv.implementationMap.at(resolvedType).at(id).first;
    llvm::FunctionType* funcType = drv.llvmModule->getFunction(rec->definer + '.' + id)->getFunctionType();

    int funcOffset = drv.implementationMap.at(resolvedType).at(id).second;

    llvm::Value* vtablePtr_ptr = drv.llvmBuilder->CreateStructGEP(args[0], vtableOffset, "vtablePtr_ptr");
    llvm::Value* vtablePtr = drv.llvmBuilder->CreateLoad(vtablePtr_ptr);
    llvm::Value* funcPtr_ptr = drv.llvmBuilder->CreateGEP(
        vtablePtr,
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(*drv.llvmContext), llvm::APInt(64, firstFuncOffset + funcOffset, false)),
        "funcPtr_ptr");
    llvm::Value* funcPtr = drv.llvmBuilder->CreateLoad(funcPtr_ptr, "funcPtr");
    llvm::Value* func = drv.llvmBuilder->CreateBitCast(funcPtr, funcType->getPointerTo(), id);
    llvm::FunctionCallee fc(funcType, func);
    args[0] = drv.llvmBuilder->CreateBitCast(args[0], funcType->getParamType(0));

    llvm::CallInst* ret = drv.llvmBuilder->CreateCall(fc, args, "ret");
    //TODO figure out why ret type is a function pointer. for now, use mutateType
    ret->mutateType(funcType->getReturnType());

    return ret;
}

llvm::Value* _if::codegen(ParserDriver& drv) {
    llvm::Value* _predicate = predicate->codegen(drv);

    llvm::BasicBlock* true_b = llvm::BasicBlock::Create(*drv.llvmContext, "if_t", drv.cur_func);
    llvm::BasicBlock* false_b = llvm::BasicBlock::Create(*drv.llvmContext, "if_f", drv.cur_func);
    llvm::BasicBlock* fi_b = llvm::BasicBlock::Create(*drv.llvmContext, "fi", drv.cur_func);

    llvm::Value* rawBool_ptr = drv.llvmBuilder->CreateStructGEP(_predicate, firstAttrOffset, "rawBool_ptr");
    llvm::Value* rawBool = drv.llvmBuilder->CreateLoad(rawBool_ptr);
    drv.llvmBuilder->CreateCondBr(rawBool, true_b, false_b);

    //cast results of true and false paths because PHI node needs both candidates to be the same type
    drv.llvmBuilder->SetInsertPoint(true_b);
    llvm::Value* true_res = tthen->codegen(drv); true_res->setName("true_res");
    llvm::Value* true_res_cast = drv.llvmBuilder->CreateBitCast(true_res, llvm::Type::getInt64PtrTy(*drv.llvmContext));
    drv.llvmBuilder->CreateBr(fi_b);

    drv.llvmBuilder->SetInsertPoint(false_b);
    llvm::Value* false_res = eelse->codegen(drv); false_res->setName("false_res");
    llvm::Value* false_res_cast = drv.llvmBuilder->CreateBitCast(true_res, llvm::Type::getInt64PtrTy(*drv.llvmContext));
    drv.llvmBuilder->CreateBr(fi_b);

    drv.llvmBuilder->SetInsertPoint(fi_b);
    llvm::PHINode* phi = drv.llvmBuilder->CreatePHI(llvm::Type::getInt64PtrTy(*drv.llvmContext), 2, "if_res");
    phi->addIncoming(true_res_cast, true_b);
    phi->addIncoming(false_res_cast, false_b);

    string resolvedType = type;
    if(type == "SELF_TYPE") resolvedType = resolveType(drv);
    return drv.llvmBuilder->CreatePointerCast(phi, drv.llvmModule->getTypeByName(resolvedType + "_c")->getPointerTo(), "if_res_cast");
}

llvm::Value* _new::codegen(ParserDriver& drv) {
    if(id == "Int") {
        return _int(0, 0).codegen(drv);
    }
    else if(id == "Bool") {
        return _bool(0, false).codegen(drv);
    }
    else if(id == "String") {
        return _string(0, "").codegen(drv);
    }
    else {
        int numAttr = drv.classMap.at(id).size();
        llvm::Value* numBytes = llvm::ConstantInt::get(
                llvm::Type::getInt64Ty(*drv.llvmContext), llvm::APInt(64, (1 + numAttr) * 8));
        llvm::Value* mallocRes = drv.llvmBuilder->CreateCall(drv.llvmModule->getFunction("malloc"), vector<llvm::Value*>{numBytes}, "mallocRes");
        llvm::Value* castedMallocRes = drv.llvmBuilder->CreateBitCast(mallocRes, drv.llvmModule->getTypeByName(id + "_c")->getPointerTo(), "castedMallocRes");
        drv.llvmBuilder->CreateCall(drv.llvmModule->getFunction(id + "..ctr"), castedMallocRes);
        return castedMallocRes;
    }
}

llvm::Value* _int::codegen(ParserDriver& drv) {
    llvm::Value* numBytes = llvm::ConstantInt::get(
        llvm::Type::getInt64Ty(*drv.llvmContext), llvm::APInt(64, 16, false)); //vtable pointer, raw value (64 bit int)
    llvm::Value* mallocRes = drv.llvmBuilder->CreateCall(drv.llvmModule->getFunction("malloc"), vector<llvm::Value*>{numBytes}, "mallocRes");
    llvm::Value* castedMallocRes = drv.llvmBuilder->CreateBitCast(mallocRes, drv.llvmModule->getTypeByName("Int_c")->getPointerTo(), "castedMallocRes");
    llvm::Value* intLiteralValue = llvm::ConstantInt::get(
        llvm::Type::getInt64Ty(*drv.llvmContext), llvm::APInt(64, value, false));
    drv.llvmBuilder->CreateCall(drv.llvmModule->getFunction("Int..ctr"), vector<llvm::Value*>{castedMallocRes, intLiteralValue});
    return castedMallocRes;
}

llvm::Value* _bool::codegen(ParserDriver& drv) {
    llvm::Value* numBytes = llvm::ConstantInt::get(
            llvm::Type::getInt64Ty(*drv.llvmContext), llvm::APInt(64, 9, false)); //vtable pointer, raw value (1 bit int)
    llvm::Value* mallocRes = drv.llvmBuilder->CreateCall(drv.llvmModule->getFunction("malloc"), vector<llvm::Value*>{numBytes}, "mallocRes");
    llvm::Value* castedMallocRes = drv.llvmBuilder->CreateBitCast(mallocRes, drv.llvmModule->getTypeByName("Bool_c")->getPointerTo(), "castedMallocRes");
    int intVal;
    if(value) intVal = 1;
    else intVal = 0;
    llvm::Value* intLiteralValue = llvm::ConstantInt::get(
        llvm::Type::getInt1Ty(*drv.llvmContext), llvm::APInt(1, intVal, false));
    drv.llvmBuilder->CreateCall(drv.llvmModule->getFunction("Bool..ctr"), vector<llvm::Value*>{castedMallocRes, intLiteralValue});
    return castedMallocRes;
}

llvm::Value* _string::codegen(ParserDriver& drv) {
    llvm::Value* numBytes = llvm::ConstantInt::get(
        llvm::Type::getInt64Ty(*drv.llvmContext), llvm::APInt(64, 32, false)); //i8*, i64, i64, i64 so 8 * 4 = 32 bytes
    llvm::Value* mallocRes_LLVMString = drv.llvmBuilder->CreateCall(drv.llvmModule->getFunction("malloc"), vector<llvm::Value*>{numBytes}, "mallocRes_LLVMString");
    llvm::Value* castedMallocRes_LLVMString = drv.llvmBuilder->CreateBitCast(mallocRes_LLVMString, drv.llvmModule->getTypeByName("LLVMString")->getPointerTo(), "castedMallocRes_LLVMString");
    llvm::Value* stringPtr = drv.strLits.at(value).second;
    drv.llvmBuilder->CreateCall(drv.llvmModule->getFunction("LLVMString..ctr"), vector<llvm::Value*>{castedMallocRes_LLVMString, stringPtr});

    numBytes = llvm::ConstantInt::get(
        llvm::Type::getInt64Ty(*drv.llvmContext), llvm::APInt(64, 16, false)); //vtable pointer, raw LLVMString
    llvm::Value* mallocRes = drv.llvmBuilder->CreateCall(drv.llvmModule->getFunction("malloc"), vector<llvm::Value*>{numBytes}, "mallocRes");
    llvm::Value* castedMallocRes = drv.llvmBuilder->CreateBitCast(mallocRes, drv.llvmModule->getTypeByName("String_c")->getPointerTo(), "castedMallocRes");
    llvm::Value* loadedLLVMString = drv.llvmBuilder->CreateLoad(castedMallocRes_LLVMString, "loaded_LLVMString");
    drv.llvmBuilder->CreateCall(drv.llvmModule->getFunction("String..ctr"), vector<llvm::Value*>{castedMallocRes, loadedLLVMString});

    return castedMallocRes;
}

llvm::Value* _block::codegen(ParserDriver& drv) {
    for(vector<_expr*>::iterator it = body.begin(); it != body.end() - 1; ++it) {
        (*it)->codegen(drv);
    }
    return (*(body.end() - 1))->codegen(drv);
}

llvm::Value* _let::codegen(ParserDriver& drv) {
    map<string, llvm::Value*>& localsMap = drv.currentMethodEnv->localsMap;
    //call alloca for each identifier, and we can codegen the init expression in _letBinding
    for(_letBinding* binding : bindingList) {
        string resolvedType = binding->type;
        if(resolvedType == "SELF_TYPE") {
            resolvedType = drv.currentClassEnv->id;
        }
        llvm::Type* llvmType = drv.llvmModule->getTypeByName(resolvedType + "_c");

        localsMap[selfEnv->id + '.' + binding->id] = drv.llvmBuilder->CreateAlloca(
            llvmType->getPointerTo(), 0, nullptr, id + '.' + binding->id);
    }

    drv.top = (letCaseEnv*)selfEnv;
    for(_letBinding* binding : bindingList) {
        string resolvedType = binding->type;
        if(resolvedType == "SELF_TYPE") {
            resolvedType = drv.currentClassEnv->id;
        }
        llvm::Type* llvmType = drv.llvmModule->getTypeByName(resolvedType + "_c");
        llvm::Value* storeAtEnd;
        if(resolvedType == "Int" || resolvedType == "Bool" || resolvedType == "String") {
            if(resolvedType == "String") {
                if(!binding->optInit) {
                    binding->optInit = new _string(0, ""); //random line number which will never be used, and default "" value
                }
                storeAtEnd = binding->optInit->codegen(drv);
            }
            else if(resolvedType == "Int") {
                if(!binding->optInit) {
                    binding->optInit = new _int(0, 0); //random line number which will never be used, and default 0 value
                }
                storeAtEnd = binding->optInit->codegen(drv);
            }
            else if(resolvedType == "Bool") {
                if(!binding->optInit) {
                    binding->optInit = new _bool(0, false); //random line number which will never be used, and default false value
                }
                storeAtEnd = binding->optInit->codegen(drv);
            }
        }
        else {
            if(binding->optInit) {
                llvm::Value* initExprRes = binding->optInit->codegen(drv);
                initExprRes->setName("initExprRes");
                storeAtEnd = drv.llvmBuilder->CreateBitCast(initExprRes, llvmType->getPointerTo(), "castedInit");
            }
            else {
                storeAtEnd = llvm::Constant::getNullValue(llvmType->getPointerTo());
            }
        }
        drv.llvmBuilder->CreateStore(storeAtEnd, localsMap[selfEnv->id + '.' + binding->id]);
    }
    llvm::Value* retThis = body->codegen(drv);
    drv.top = drv.top->prevLetCase;

    return retThis;
}

/**
 * identifier resolution from lval -> rval needs to happen in this order: local(letCase) vars, method arguments, then class attributes
 * we are currently under the assumption that the identifier exists in one of these 3 categories
 * @param drv
 * @return
 */
llvm::Value* _id::codegen(ParserDriver& drv) {
    llvm::Value* retThis;
    bool found = false;

    if(value == "self") {
        return drv.cur_func->getArg(0);
    }

    if(drv.top != nullptr) {//locals - letCase
        map<string, llvm::Value*>::iterator searchResIt = drv.currentMethodEnv->localsMap.find(drv.top->id + '.' + value);
        if(searchResIt != drv.currentMethodEnv->localsMap.end()) {
            llvm::Value* doublePtr = searchResIt->second;
            retThis = drv.llvmBuilder->CreateLoad(doublePtr); retThis->setName(drv.top->id + '.' + value + "_loaded");
            found = true;
        }
    }
    if(!found) {//locals - methodParams
        map<string, llvm::Value*>::iterator searchResIt = drv.currentMethodEnv->localsMap.find("methodParam." + value);
        if(searchResIt != drv.currentMethodEnv->localsMap.end()) {
            llvm::Value* doublePtr = searchResIt->second;
            retThis = drv.llvmBuilder->CreateLoad(doublePtr); retThis->setName("methodParam." + value + "_loaded");
            found = true;
        }
    }
    if(!found) {//attributes
        int attrOffset = drv.classMap.at(drv.currentClassEnv->id).at(value).second;
        llvm::Value* attrPtr_ptr = drv.llvmBuilder->CreateStructGEP(drv.cur_func->getArg(0), firstAttrOffset + attrOffset, value + "Ptr_ptr");
        retThis = drv.llvmBuilder->CreateLoad(attrPtr_ptr, value + "Ptr");
    }
    return retThis;
}

llvm::Value* _assign::codegen(ParserDriver& drv) {
    llvm::Value* rhs_val = rhs->codegen(drv);

    llvm::Value* lhs_doublePtr;
    bool found = false;
    string lhsType;

    //locals first, then methodParams, then attributes
    if(drv.top != nullptr) {
        map<string, llvm::Value*>::iterator searchResIt = drv.currentMethodEnv->localsMap.find(drv.top->id + '.' + id);
        if(searchResIt != drv.currentMethodEnv->localsMap.end()) {
            lhs_doublePtr = searchResIt->second;
            lhsType = drv.top->getRec(id)->type;
            found = true;
        }
    }
    if(!found) {
        map<string, llvm::Value*>::iterator searchResIt = drv.currentMethodEnv->localsMap.find("methodParam." + id);
        if(searchResIt != drv.currentMethodEnv->localsMap.end()) {
            lhs_doublePtr = searchResIt->second;
            lhsType = drv.currentMethodEnv->getRec(id)->type;
            found = true;
        }
    }
    if(!found) {
        int attrOffset = drv.classMap.at(drv.currentClassEnv->id).at(id).second;
        lhs_doublePtr = drv.llvmBuilder->CreateStructGEP(drv.cur_func->getArg(0), firstAttrOffset + attrOffset);
        lhsType = drv.classMap.at(drv.currentClassEnv->id).at(id).first->type;
    }

    llvm::Value* rhs_cast = drv.llvmBuilder->CreateBitCast(rhs_val, drv.llvmModule->getTypeByName(lhsType + "_c")->getPointerTo());
    drv.llvmBuilder->CreateStore(rhs_cast, lhs_doublePtr);
    return rhs_val;
}