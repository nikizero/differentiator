#include "../incude/appcontext.h"
#include "../incude/io.h"
#include "../incude/tex.h"
#include "../incude/differentiator.h"
#include "../incude/tree.h"
#include <time.h>

int main() {
    srand(time(nullptr));
    AppContext context = {};
    InitializeContext(&context);
    AskDifferentiationVariable(&context);
    TexDumpBegin();

    TREE_DUMP(&context, context.originalTree);
    PerformDifferentiation(&context);
    ComputeTaylorSeries(&context);
    TREE_DUMP(&context, context.diffTree);

    TexDestroy();
    TreeDtor(&context.diffTree, &context);
    TreeDtor(&context.originalTree, &context);

    return 0;
}
