#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <memory>
#include <cctype>
#include <map>
#include <string>
#include <utility>
#include <vector>

using namespace std;

class ExprAST {
public:
    virtual ~ExprAST() = default;
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

static int currToken;
static int getNextToken();

unique_ptr<ExprAST> logError(const char *str);

unique_ptr<PrototypeAST> logErrorP(const char *str);

#endif