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

static unique_ptr<ExprAST> parseNumberExpr() {

}

static unique_ptr<ExprAST> parseParenExpr() {

}

static unique_ptr<ExprAST> parseIdentifierExpr() {

}

static unique_ptr<ExprAST> parsePrimary() {

}

static unique_ptr<ExprAST> parseBinOpRHS() {

}

static int getTokenPrecedence() {

}

static unique_ptr<ExprAST> parseExpression() {

}

static unique_ptr<PrototypeAST> parsePrototype() {

}

static unique_ptr<FunctionAST> parseFunctionDefinition() {

}

static unique_ptr<PrototypeAST> parseExtern() {

}

static unique_ptr<FunctionAST> parseTopLevelExpr() {

}

static void handleDefinition() {

}

static void handleExtern() {

}

static void handleTopLevelExpression() {

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