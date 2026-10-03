#include "parser.h"
#include "lexer.h"

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

static int getNextToken() {
    return currToken = getToken();
}

unique_ptr<ExprAST> logError(const char *str) {
    fprintf(stderr, "Error: %s\n", str);
    return nullptr;
}

unique_ptr<PrototypeAST> logErrorP(const char *str) {
    logError(str);
    return nullptr;
}