#include "../incude/tex.h"
#include "../incude/differentiator.h"
#include "../incude/tree.h"
#include <cstdio>
#include <cstdlib>

FILE* tex_file = NULL;

void TexDumpBegin() {
    tex_file = fopen("out.tex", "w");
    if (!tex_file) {
        fprintf(stderr, "Error: Cannot open out.tex\n");
        return;
    }
    fprintf(tex_file,
        "\\documentclass[a4paper,14pt]{extarticle}\n"
        "\\usepackage{graphicx}\n"
        "\\usepackage[utf8]{inputenc}\n"
        "\\usepackage[russian]{babel}\n"
        "\\usepackage{amsmath}\n"
        "\\usepackage{amsfonts}\n"
        "\\usepackage{mathtext}\n"
        "\\begin{document}\n"
    );
}

int TexDestroy() {
    fprintf(tex_file, "\\end{document}\n");
    fclose(tex_file);
    tex_file = NULL;
    return 0;
}

void TexDump(AppContext* context, Node* node1, Node* node2, const char* phrase) {
    const char* varName = context->variableName;
    fprintf(tex_file, "%s\n", phrase);
    fprintf(tex_file, "\\begin{center}$");
    if (node1 == node2) {
        fprintf(tex_file, "f(%s) = ", varName);
        DumpNode(node1, context->variableNames, STANDART_TEX);
    } else {
        fprintf(tex_file, " ");
        DumpNode(node1, context->variableNames, STANDART_TEX);
        fprintf(tex_file, "' = ");
        DumpNode(node2, context->variableNames, STANDART_TEX);
    }
    fprintf(tex_file, "$\\end{center}\\ \n");
}

FILE* GetFileName() {
    return tex_file;
}

void DumpNode(Node* node, char variableNames[MAX_COUNT_VARIABLE][MAX_VARIABLE_LEN], FILE* file) {
    FILE* buf_file = tex_file;
    if (file != 0) {
        tex_file = file;
    }

    if (!node) return;
    
    if (node->type == NUM) {
        if (CmpDouble((int)node->data.num, node->data.num) == 0)
            fprintf(tex_file, "%d", (int)node->data.num);
        else
            fprintf(tex_file, "%lf", node->data.num);
    } else if (node->type == VAR) {
        fprintf(tex_file, "%s", variableNames[node->data.ind_var_arr]);
    } else if (node->type == OP) {
        if (node->data.op == ADD || node->data.op == SUB || node->data.op == MULT || node->data.op == DIV || node->data.op == POW) {
            if (node->data.op == DIV) {
                fprintf(tex_file, "\\frac{");
                DumpNode(node->left, variableNames, file);
                fprintf(tex_file, "}{");
                DumpNode(node->right, variableNames, file);
                fprintf(tex_file, "}");
            } else if (node->data.op == MULT) {
                DumpNode(node->left, variableNames, file);
                fprintf(tex_file, "\\cdot ");
                DumpNode(node->right, variableNames, file);
            } else if (node->data.op == POW) {
                DumpNode(node->left, variableNames, file);
                fprintf(tex_file, "^");
                fprintf(tex_file, "{");
                DumpNode(node->right, variableNames, file);
                fprintf(tex_file, "}");
            } else {
                fprintf(tex_file, "(");
                DumpNode(node->left, variableNames, file);
                fprintf(tex_file, "%s", OperationArray[node->data.op - 1].op_char_name);
                DumpNode(node->right, variableNames, file);
                fprintf(tex_file, ")");
            }
        } else {
            fprintf(tex_file, "\\%s  ", OperationArray[node->data.op - 1].op_char_name);
            DumpNode(node->right, variableNames, file);
        }
    }

    tex_file = buf_file;
}
