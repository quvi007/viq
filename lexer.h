// Lots of rooms for improvement

#ifndef LEXER_H
#define LEXER_H

#include <iostream>

using namespace std;

enum Token {
    tok_eof = -1,
    tok_def = -2,
    tok_extern = -3,
    tok_identifier = -4,
    tok_number = -5,
};

static string identifierStr;
static double numVal;

static int getToken() {
    static int currChar = ' ';
    while (isspace(currChar)) {
        currChar = getchar();
    }
    if (isalpha(currChar)) {
        identifierStr = currChar;
        while (isalnum((currChar = getchar()))) {
            identifierStr += currChar;
        }
        if (identifierStr == "def") {
            return tok_def;
        } else if (identifierStr == "extern") {
            return tok_extern;
        }
        return tok_identifier;
    }
    if (isdigit(currChar) || currChar == '.') {
        string numStr;
        do {
            numStr += currChar;
            currChar = getchar();
        } while (isdigit(currChar) || currChar == '.');
        numVal = strtod(numStr.c_str(), 0);
        return tok_number;
    }
    if (currChar == '#') {
        do {
            currChar = getchar();
        } while (currChar != EOF && currChar != '\n' && currChar != '\r');
        if (currChar != EOF) {
            return getToken();
        }
    }
    if (currChar == EOF) {
        return tok_eof;
    }
    int thisChar = currChar;
    currChar = getchar();
    return thisChar;
}

#endif