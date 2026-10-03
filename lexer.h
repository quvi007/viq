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

static int getToken();

#endif