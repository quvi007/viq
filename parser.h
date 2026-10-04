#ifndef PARSER_H
#define PARSER_H

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
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>
using namespace llvm;

#include "lexer.h"

class ExprAST {
public:
    virtual ~ExprAST() = default;
    virtual Value *codegen() = 0;
};

class NumberExprAST : public ExprAST {
    double val;
public:
    NumberExprAST(double val);
};

class VariableExprAST : public ExprAST {
    string name;
public:
    VariableExprAST(const string &name);
};

class BinaryExprAST : public ExprAST {
    char op;
    unique_ptr<ExprAST> LHS, RHS;
public:
    BinaryExprAST(char op, unique_ptr<ExprAST> LHS, unique_ptr<ExprAST> RHS);
};

class CallExprAST : public ExprAST {
    string callee;
    vector<unique_ptr<ExprAST>> args;
public:
    CallExprAST(const string &callee, vector<unique_ptr<ExprAST>> args);
};

class PrototypeAST {
    string name;
    vector<string> args;
public:
    PrototypeAST(const string &name, vector<string> args);
    const string &getName() const;
};

class FunctionAST {
    unique_ptr<PrototypeAST> prototype;
    unique_ptr<ExprAST> body;
public:
    FunctionAST(unique_ptr<PrototypeAST> prototype, unique_ptr<ExprAST> body);
};

NumberExprAST::NumberExprAST(double val) : val{val} {

}

VariableExprAST::VariableExprAST(const string &name) : name{name} {

}

BinaryExprAST::BinaryExprAST(char op, unique_ptr<ExprAST> LHS, unique_ptr<ExprAST> RHS) : op{op}, LHS{move(LHS)}, RHS{move(RHS)} {

}

CallExprAST::CallExprAST(const string &callee, vector<unique_ptr<ExprAST>> args) : callee{callee}, args{move(args)} {

}

PrototypeAST::PrototypeAST(const string &name, vector<string> args) : name{name}, args{move(args)} {

}

const string &PrototypeAST::getName() const {
    return name;
}

FunctionAST::FunctionAST(unique_ptr<PrototypeAST> prototype, unique_ptr<ExprAST> body) : prototype{move(prototype)}, body{move(body)} {

}

static int currToken;
static map<char, int> binopPrecedence;

static int getNextToken() {
    return currToken = getToken();
}

static int getTokenPrecedence() {
    if (!isascii(currToken)) return -1;
    int tokenPrec = binopPrecedence[currToken];
    if (tokenPrec <= 0) return -1;
    return tokenPrec;
}

unique_ptr<ExprAST> logError(const char *str) {
    fprintf(stderr, "Error: %s\n", str);
    return nullptr;
}

unique_ptr<PrototypeAST> logErrorP(const char *str) {
    logError(str);
    return nullptr;
}

static unique_ptr<ExprAST> parseNumberExpr() {
    auto result = make_unique<NumberExprAST>(numVal);
    getNextToken();
    return move(result);
}

static unique_ptr<ExprAST> parseExpression();

static unique_ptr<ExprAST> parseParenExpr() {
    getNextToken();
    auto expr = parseExpression();
    if (!expr) return nullptr;
    if (currToken != ')')
        return logError("expected ')'");
    getNextToken();
    return expr;
}

static unique_ptr<ExprAST> parseIdentifierExpr() {
    string idName = identifierStr;
    getNextToken();
    if (currToken != '(')
        return make_unique<VariableExprAST>(idName);
    getNextToken();
    vector<unique_ptr<ExprAST>> args;
    if (currToken != ')') {
        while (true) {
            if (auto arg = parseExpression()) {
                args.push_back(move(arg));
            } else {
                return nullptr;
            }
            if (currToken == ')')
                break;
            if (currToken != ',')
                return logError("Expected ')' or ',' in argument list");
            getNextToken();
        }
    }
    getNextToken();
    return make_unique<CallExprAST>(idName, move(args));
}

static unique_ptr<ExprAST> parsePrimary() {
    switch (currToken) {
        case tok_identifier:
            return parseIdentifierExpr();
        case tok_number:
            return parseNumberExpr();
        case '(':
            return parseParenExpr();
        default:
            return logError("Unknown token when expecting an expression");
    }
}

static unique_ptr<ExprAST> parseBinOpRHS(int exprPrec, unique_ptr<ExprAST> LHS) {
    while (true) {
        int tokenPrec = getTokenPrecedence();
        if (tokenPrec < exprPrec)
            return LHS;
        int binOp = currToken;
        getNextToken();
        auto RHS = parsePrimary();
        if (!RHS) return nullptr;
        int nextPrec = getTokenPrecedence();
        if (tokenPrec < nextPrec) {
            RHS = parseBinOpRHS(tokenPrec + 1, move(RHS));
            if (!RHS) return nullptr;
        }
        LHS = make_unique<BinaryExprAST>(binOp, move(LHS), move(RHS));
    }
}

static unique_ptr<ExprAST> parseExpression() {
    auto LHS = parsePrimary();
    if (!LHS) return nullptr;
    return parseBinOpRHS(0, move(LHS));
}

static unique_ptr<PrototypeAST> parsePrototype() {
    if (currToken != tok_identifier) {
        return logErrorP("Expected function name in prototype");
    }
    string funcName = identifierStr;
    getNextToken();
    if (currToken != '(') {
        return logErrorP("Expected '(' in prototype");
    }
    vector<string> argNames;
    while (getNextToken() == tok_identifier) {
        argNames.push_back(identifierStr);
    }
    if (currToken != ')') {
        return logErrorP("Expected ')' in prototype");
    }
    getNextToken();
    return make_unique<PrototypeAST>(funcName, move(argNames));
}

static unique_ptr<FunctionAST> parseFunctionDefinition() {
    getNextToken();
    auto prototype = parsePrototype();
    if (!prototype) return nullptr;
    auto expr = parseExpression();
    if (!expr) return nullptr;
    return make_unique<FunctionAST>(move(prototype), move(expr));
}

static unique_ptr<PrototypeAST> parseExtern() {
    getNextToken();
    return parsePrototype();
}

static unique_ptr<FunctionAST> parseTopLevelExpr() {
    auto expr = parseExpression();
    if (!expr) return nullptr;
    auto prototype = make_unique<PrototypeAST>("__anon_expr", vector<string>());
    return make_unique<FunctionAST>(move(prototype), move(expr));
}

static void handleDefinition() {
    if (parseFunctionDefinition()) {
        fprintf(stderr, "Parsed a function definition.\n");
    } else {
        getNextToken();
    }
}

static void handleExtern() {
    if (parseExtern()) {
        fprintf(stderr, "Parsed an extern.\n");
    } else {
        getNextToken();
    }
}

static void handleTopLevelExpression() {
    if (parseTopLevelExpr()) {
        fprintf(stderr, "Parsed a top-level expr\n");
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