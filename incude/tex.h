#pragma once
#include "appcontext.h"
#include <cstdio>

void DumpNode(Node* node, char variableNames[MAX_COUNT_VARIABLE][MAX_VARIABLE_LEN], FILE* file);
void TexDumpBegin();
int TexDestroy();
void TexDump(AppContext* context, Node* node1, Node* node2, const char* phrase);
FILE* GetFileName();
