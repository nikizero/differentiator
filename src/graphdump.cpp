#include "../incude/tree.h"
#include "../incude/appcontext.h"
#include <stdio.h>
#include <cstdlib>

static FILE* graph_file = NULL;
static FILE* logfile = NULL;

static void PrintGraphNode(Node* node, size_t* number_of_node, const char* color, char Variables[MAX_COUNT_VARIABLE][MAX_VARIABLE_LEN]);
static void PrintGraphEdge(size_t from, size_t to, const char* color);
static void NodeDump(Node* node, size_t* number_of_node, const char* color, char Variables[MAX_COUNT_VARIABLE][MAX_VARIABLE_LEN]);

void TreeDumpFunction(AppContext* context, Node* node, const char* path, const char* signature, unsigned line) {
    if (!logfile)
        logfile = fopen("logfile.html", "a");

    fprintf(logfile, "<pre>\n");
    fprintf(logfile, "<font size = \"+1\">path: %s\n</font>", path);
    fprintf(logfile, "<font size = \"+1\">in function: %s\n</font>", signature);
    fprintf(logfile, "<font size = \"+1\">line: %u\n</font>", line);

    if (context->originalTree.status != TREE_OK) {
        extern const TREE_STATUS ErrorMas[];
        for (int j = 0; j < NUMBER_OF_ERROR; j++) {
            if ((context->originalTree.status & (1 << j))) {
                fprintf(logfile, "<font color = \"red\">ERROR: %s\n</font>", ErrorMas[j+1].NameError);
            }
        }
    } else {
        graph_file = fopen("dotfile1.dot", "wb");
        static int dump_number = 1;
        fprintf(graph_file, "digraph struct {bgcolor=RosyBrown rankdir = HR\n\n\n");
        fprintf(graph_file, "label = \"tree_dump from function %s, Tree/%s:%u\";\n", signature, path, line);

        size_t number_of_node = 0;
        if (node != nullptr)
            NodeDump(node, &number_of_node, "red", context->variableNames);

        fprintf(graph_file, "\n\n}");
        fclose(graph_file);

        char shell_command[MAX_COMMAND_LENGTH];
        snprintf(shell_command, sizeof(shell_command), "dot -Tpng dotfile1.dot -o graph%d.png", dump_number);
        system(shell_command);

        fprintf(logfile, "<img width=\"1100px\" height=\"600px\" src=\"graph%d.png\">", dump_number);
        fprintf(logfile, ".\n.\n.\n");
        dump_number++;
    }

    fclose(logfile);
    logfile = NULL;
}

static void PrintGraphNode(Node* node, size_t* number_of_node, const char* color, char Variables[MAX_COUNT_VARIABLE][MAX_VARIABLE_LEN]) {
    extern const Operation OperationArray[];
    if (node->type == OP) {
        fprintf(graph_file, "node%zu[shape=record, style=filled, fillcolor=\"%s\", label=\" {ADDRESS: %p | DATA: %s}\"];\n",
                *number_of_node, color, node, OperationArray[node->data.op - 1].op_char_name);
    } else if (node->type == NUM) {
        fprintf(graph_file, "node%zu[shape=record, style=filled, fillcolor=\"%s\", label=\" {ADDRESS: %p | DATA: %lf}\"];\n",
                *number_of_node, color, node, node->data.num);
    } else if (node->type == VAR) {
        fprintf(graph_file, "node%zu[shape=record, style=filled, fillcolor=\"%s\", label=\" {ADDRESS: %p | DATA: %s}\"];\n",
                *number_of_node, color, node, Variables[node->data.ind_var_arr]);
    }
}

static void PrintGraphEdge(size_t from, size_t to, const char* color) {
    fprintf(graph_file, "node%zu->node%zu [color = \"%s\"];\n", from, to, color);
}

static void NodeDump(Node* node, size_t* number_of_node, const char* color, char Variables[MAX_COUNT_VARIABLE][MAX_VARIABLE_LEN]) {
    PrintGraphNode(node, number_of_node, color, Variables);
    size_t current_number = *number_of_node;

    if (node->left) {
        PrintGraphEdge(current_number, ++(*number_of_node), "lime");
        NodeDump(node->left, number_of_node, "lime", Variables);
    }
    if (node->right) {
        PrintGraphEdge(current_number, ++(*number_of_node), "aqua");
        NodeDump(node->right, number_of_node, "aqua", Variables);
    }
}
