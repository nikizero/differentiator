#pragma once
#include "tree.h"

struct AppContext {
    Tree originalTree;
    Tree diffTree;
    char variableName[MAX_VARIABLE_LEN];
    size_t variableIndex;
    char variableNames[MAX_COUNT_VARIABLE][MAX_VARIABLE_LEN];
    Node* InitNodes[MAX_TOKENS];
};
