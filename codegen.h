#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"

using namespace llvm;

static unique_ptr<LLVMContext> TheContext;
static unique_ptr<IRBuilder<>> Builder;
static unique_ptr<Module> TheModule;
static map<string, Value *> NamedValues;

Value *logErrorV(const char *str) {
    logError(str);
    return nullptr;
}

Value *NumberExprAST::codegen() {
    return ConstantFP::get(*TheContext, APFloat(val));
}

Value *VariableExprAST::codegen() {
    Value *v = NamedValues[name];
    if (!v) logErrorV("Unknown variable name");
    return v;
}

Value *BinaryExprAST::codegen() {
    Value *l = LHS->codegen();
    Value *r = RHS->codegen();
    if (!l || !r) return nullptr;
    switch (op) {
        case '+':
            return Builder->CreateFAdd(l, r, "addtmp");
        case '-':
            return Builder->CreateFSub(l, r, "subtmp");
        case '*':
            return Builder->CreateFMul(l, r, "multmp");
        case '<':
            l = Builder->CreateFCmpULT(l, r, "cmptmp");
            return Builder->CreateUIToFP(l, Type::getDoubleTy(*TheContext), "booltmp");
        default:
            return logErrorV("invalid binary operator");
    }
}

Value *CallExprAST::codegen() {
    Function *calleeFunc = TheModule->getFunction(callee);
    if (!calleeFunc) {
        return logErrorV("Unknown function referenced");
    }
    if (calleeFunc->arg_size() != args.size()) {
        return logErrorV("Incorrect # arguments passed");
    }
    vector<Value *> argsV;
    for (int i = 0; i < (int)args.size(); ++i) {
        Value *argV = args[i]->codegen();
        if (!argV) return nullptr;
        argsV.push_back(argV);
    }
    return Builder->CreateCall(calleeFunc, argsV, "calltmp");
}

Function *PrototypeAST::codegen() {
    vector<Type *> argTypes(args.size(), Type::getDoubleTy(*TheContext));
    FunctionType *funcType = FunctionType::get(Type::getDoubleTy(*TheContext), argTypes, false);
    Function *func = Function::Create(funcType, Function::ExternalLinkage, name, TheModule.get());
    int i = 0;
    for (auto &arg : func->args()) {
        arg.setName(args[i++]);
    }
    return func;
}

Function *FunctionAST::codegen() {
    Function *func = TheModule->getFunction(prototype->getName());
    if (!func) func = prototype->codegen();
    if (!func) return nullptr;
    if (!func->empty()) {
        return (Function *)logErrorV("Function cannot be redefined");
    }
    BasicBlock *bb = BasicBlock::Create(*TheContext, "entry", func);
    Builder->SetInsertPoint(bb);
    NamedValues.clear();
    for (auto &arg : func->args()) {
        NamedValues[string(arg.getName())] = &arg;
    }
    Value *retVal = body->codegen();
    if (!retVal) {
        func->eraseFromParent();
        return nullptr;
    }
    Builder->CreateRet(retVal);
    verifyFunction(*func);
    return func;
}

static void InitializeModule() {
    TheContext = make_unique<LLVMContext>();
    TheModule = make_unique<Module>("viq jit", *TheContext);
    Builder = make_unique<IRBuilder<>>(*TheContext);
}

static void handleDefinition() {
    if (auto funcAST = parseFunctionDefinition()) {
        if (auto *funcIR = funcAST->codegen()) {
            fprintf(stderr, "Read function definition:");
            funcIR->print(errs());
            fprintf(stderr, "\n");
        }
    } else {
        getNextToken();
    }
}

static void handleExtern() {
    if (auto prototypeAST = parseExtern()) {
        if (auto *prototypeIR = prototypeAST->codegen()) {
            fprintf(stderr, "Read extern: ");
            prototypeIR->print(errs());
            fprintf(stderr, "\n");
        }
    } else {
        getNextToken();
    }
}

static void handleTopLevelExpression() {
    if (auto funcAST = parseTopLevelExpr()) {
        if (auto *funcIR = funcAST->codegen()) {
            fprintf(stderr, "Read top-level expression:");
            funcIR->print(errs());
            fprintf(stderr, "\n");
            funcIR->eraseFromParent();
        }
    } else {
        getNextToken();
    }
}

static void mainLoop() {
    while (true) {
        fprintf(stderr, "ready> ");
        switch (currToken) {
            case tok_eof:
                return;
            case ';':
                getNextToken();
                break;
            case tok_def:
                handleDefinition();
                break;
            case tok_extern:
                handleExtern();
                break;
            default:
                handleTopLevelExpression();
                break;
        }
    }
}

#endif