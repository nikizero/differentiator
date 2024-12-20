#pragma once
#include "../incude/appcontext.h"

void InitializeContext(AppContext* context);
void AskDifferentiationVariable(AppContext* context);
void ComputeTaylorSeries(AppContext* context);
void FileInput(FILE* file, Text* data);
