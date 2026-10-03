#include <bits/stdc++.h>
#include "lexer.h"
#include "parser.h"

using namespace std;

int main(int argc, char *argv[]) {
    binopPrecedence['<'] = 10;
    binopPrecedence['+'] = 20;
    binopPrecedence['-'] = 20;
    binopPrecedence['*'] = 40;

    fprintf(stderr, "ready> ");
    getNextToken();
    mainLoop();
    return 0;
}
