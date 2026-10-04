#include <bits/stdc++.h>
#include "codegen.h"

int main(int argc, char *argv[]) {
    binopPrecedence['<'] = 10;
    binopPrecedence['+'] = 20;
    binopPrecedence['-'] = 20;
    binopPrecedence['*'] = 40;

    fprintf(stderr, "ready> ");
    getNextToken();
    InitializeModule();
    mainLoop();
    TheModule->print(errs(), nullptr);
    return 0;
}
