#include "../incude/io.h"
#include "../incude/tree.h"
#include "../incude/differentiator.h"
#include "../incude/appcontext.h"
#include <cstdio>
#include <cstdlib>

void FileInput(FILE* file, Text* data) {
    data->BufSize = GetFileSize(file);
    data->Buf = (char*)calloc(data->BufSize+1, sizeof(char));
    fread(data->Buf, sizeof(char), data->BufSize, file);
    data->Buf[data->BufSize] = '\0';
    fclose(file);
}

void InitializeContext(AppContext* context) {
    TreeCtor(&context->originalTree);

    FILE* file_input = fopen("input.txt", "r");
    if (!file_input) {
        fprintf(stderr, "Error: Cannot open input.txt\n");
        return;
    }

    Text data = {};
    FileInput(file_input, &data);

    Read read = {};
    TreeRead(&context->originalTree, &data, &read, context->variableNames, *context);
    
}

void AskDifferentiationVariable(AppContext* context) {
    printf("On which variable do you want to differentiate?\n");
    scanf("%s", context->variableName);

    if (!FindVariable(&context->variableIndex, context->variableName, context->variableNames)) {
        fprintf(stderr, "There is no such variable in your example.\n");
        return;
    }
}

void ComputeTaylorSeries(AppContext* context) {
    double x_point = 0;
    printf("\nAt which point do you want the Taylor expansion?\n");
    scanf("%lf", &x_point);

    GetTaylor(context, x_point);
}
