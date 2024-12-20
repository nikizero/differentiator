#include "../incude/differentiator.h"
#include "../incude/tex.h"
#include "../incude/tree.h"
#include "../incude/dsl.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

Node* D_NUM(double number, AppContext* context) {
    Data data = {};
    data.num = number;
    return CreateNode(data, NUM, nullptr, nullptr, *context);
}

const char* GetRandomPhrase() {
    int phraze_num = rand() % (int)(sizeof(PHRAZES)/sizeof(PHRAZES[0]));
    return PHRAZES[phraze_num];
}

int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

static Node* CopyNode(Node* node, AppContext* context) {
    if (!node) return nullptr;

    Node* new_node = (Node*)calloc(1, sizeof(Node));
    InsertNode(new_node, *context);

    new_node->type = node->type;
    new_node->data = node->data;

    if (node->left)
        new_node->left = CopyNode(node->left, context);

    if (node->right)
        new_node->right = CopyNode(node->right, context);

    if (new_node->left)
        new_node->left->parent = new_node;

    if (new_node->right)
        new_node->right->parent = new_node;

    return new_node;
}

#define DIFF_L Diff(context, node->left, ind_param, derivative) 
#define DIFF_R Diff(context, node->right, ind_param, derivative) 
#define COPY_L CopyNode(node->left, context)
#define COPY_R CopyNode(node->right, context)

static Node* DiffPow(AppContext* context, Node* node, size_t ind_param, bool derivative) {
    assert(node && node->left && node->right);

    Node* result = nullptr;

    if (!ContainsVar(node->right)) {
        Node* powNode = D_POW(COPY_L, D_NUM(node->right->data.num - 1, context));
        result = D_MULT(D_MULT(D_NUM(node->right->data.num, context), powNode), DIFF_L);
    } else if (!ContainsVar(node->left)) {
        result = D_MULT(
                    D_MULT(D_POW(COPY_L, COPY_R), D_NUM(log(node->left->data.num), context)),
                    DIFF_R
                 );
    } else {
        result = D_MULT(
                    D_POW(COPY_L, COPY_R),
                    D_ADD(
                        D_MULT(D_LN(nullptr, COPY_L), DIFF_R),
                        D_MULT(D_DIV(DIFF_L, COPY_L), COPY_R)
                    )
                 );
    }

    return result;
}

Node* Diff(AppContext* context, Node* node, size_t ind_param, bool derivative) {
    if (node->type == NUM)
        return D_NUM(0.0, context);

    if (node->type == VAR) {
        if (node->data.ind_var_arr == ind_param)
            return D_NUM(1.0, context);
        else
            return D_NUM(0.0, context);
    }

    Node* result = nullptr;

    switch (node->data.op) {
        case ADD:
            result = D_ADD(DIFF_L, DIFF_R);
            break;
        case SUB:
            result = D_SUB(DIFF_L, DIFF_R);
            break;
        case MULT:
            result = D_ADD(
                        D_MULT(DIFF_L, COPY_R),
                        D_MULT(COPY_L, DIFF_R)
                     );
            break;
        case DIV:
            result = D_DIV(
                        D_SUB(
                            D_MULT(DIFF_L, COPY_R),
                            D_MULT(COPY_L, DIFF_R)
                        ),
                        D_POW(COPY_R, D_NUM(2.0, context))
                     );
            break;
        case SIN:
            result = D_MULT(D_COS(nullptr, COPY_R), DIFF_R);
            break;
        case COS:
            result = D_MULT(
                        D_MULT(D_SIN(nullptr, COPY_R), DIFF_R),
                        D_NUM(-1.0, context)
                     );
            break;
        case TG:
            result = D_MULT(
                        D_DIV(D_NUM(1.0, context), D_POW(D_COS(nullptr, COPY_R), D_NUM(2.0, context))),
                        DIFF_R
                     );
            break;
        case CTG:
            result = D_MULT(
                        D_MULT(
                            D_DIV(D_NUM(1.0, context), D_POW(D_SIN(nullptr, COPY_R), D_NUM(2.0, context))),
                            DIFF_R
                        ),
                        D_NUM(-1.0, context)
                     );
            break;
        case LN:
            result = D_MULT(D_DIV(D_NUM(1.0, context), COPY_R), DIFF_R);
            break;
        case LG:
            result = D_MULT(
                        D_DIV(
                            D_NUM(1.0, context),
                            D_MULT(D_NUM(log(10.0), context), COPY_R)
                        ),
                        DIFF_R
                     );
            break;
        case POW:
            result = DiffPow(context, node, ind_param, derivative);
            break;
        default:
            printf("Error: unknown operation in Diff\n");
            return nullptr;
    }

    FILE* f = GetFileName();
    if (derivative && f) {
        Tree tree = {.root = result};

        fprintf(f, "%s\n", GetRandomPhrase());
        fprintf(f, "\\begin{center}$(");
        DumpNode(node, context->variableNames, STANDART_TEX);
        fprintf(f, ")' = ");

        DumpNode(tree.root, context->variableNames, STANDART_TEX);
        fprintf(f, "$\\end{center}\\ \n");

        Optimize(&tree, context, PRINT_OPTIMIZE);
    }

    return result;
}

void GetTaylor(AppContext* context, double x_dot) {
    FILE* f = GetFileName();
    if (!f) return;

    Tree* tree = &context->originalTree;
    Node* root = tree->root;

    const size_t degree = 3;
    double taylor_coeffs[degree + 1] = {};

    Node* cur_node = root;

    for (size_t i = 0; i <= degree; i++) {
        double val = Eval(cur_node, x_dot);
        taylor_coeffs[i] = val;

        Node* result = Diff(context, cur_node, context->variableIndex, false);
        tree->root = result;

        Optimize(tree, context, NO_PRINT_OPTIMIZE);
        cur_node = result;
    }

    fprintf(f, "\\subsection{Ответ}\\ \\newline\n");
    fprintf(f, "Разложение ряда Тейлора в точке %d:", (int)x_dot);
    fprintf(f, "\\begin{center}\n");
    fprintf(f, "$f(%s) = ", context->variableName);

    for (size_t j = 0; j <= 3; j++) {
        if (CmpDouble(taylor_coeffs[j],0) == 0)
            continue;

        if (j == 0)
            fprintf(f, "\\frac{%lg}{%d} + ", taylor_coeffs[j], factorial((int)j));
        else if (CmpDouble(x_dot,0) == 0)
            fprintf(f, "\\frac{%lg}{%d} (%s)^{%lu} + ", taylor_coeffs[j], factorial((int)j), context->variableName, j);
        else
            fprintf(f, "\\frac{%lg}{%d} (%s - %d)^{%lu} + ", taylor_coeffs[j], factorial((int)j), context->variableName, (int)x_dot, j);
    }

    fprintf(f, "o((%s - %d)^3)$ \\end{center}\\ \n", context->variableName, (int)x_dot);
}

void PerformDifferentiation(AppContext* context) {
    TreeCtor(&context->diffTree);

    TexDump(context, context->originalTree.root, context->originalTree.root, "\\ Производная Функции \\");

    context->diffTree.root = Diff(context, context->originalTree.root, context->variableIndex, true);

    Optimize(&context->diffTree, context, NO_PRINT_OPTIMIZE);

    TexDump(context, context->originalTree.root, context->diffTree.root, "В результате получаем: \\");
}
