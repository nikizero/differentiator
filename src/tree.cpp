#include "../incude/tree.h"
#include "../incude/appcontext.h"
#include "../incude/tex.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>

void InsertNode(Node* node, AppContext context) {
    for (int i = 0; i < MAX_TOKENS; i++) {
        if (context.InitNodes[i] == 0) {
            context.InitNodes[i] = node;
            break;
        }
    }
}

void RemoveAllOccurrences(Node* node, AppContext context) {
    for (int i = 0; i < MAX_TOKENS; i++) {
        if (context.InitNodes[i] == node) {
            context.InitNodes[i] = nullptr; 
        }
    }
}

void FreeNodes(AppContext context) {
    for (int i = 0; i < MAX_TOKENS; i++) {
        if (context.InitNodes[i] != 0) {
            RemoveAllOccurrences(context.InitNodes[i], context);
            free(context.InitNodes[i]);
        }
    }
}

void TreeCtor(Tree* tree) {
    tree->root = nullptr;
    tree->size = 0;
    tree->status = TREE_OK;
}

int TreeDtor(Tree* tree, AppContext* context) {
    if (tree->root != nullptr) {
        TreeDelete(tree, tree->root, context);
        tree->root = nullptr;
    }
    tree->size = 0;
    tree->status = TREE_OK;
    return tree->status;
}

void ReConnect(Node** parent_ref, Node* node, Node* child) {
    if (!parent_ref) return;
    if (!node) return;
    if (*parent_ref == node) {
        *parent_ref = child;
        if (child) {
            child->parent = nullptr;
        }
        return;
    }
    Node* parent_node = *parent_ref;
    if (!parent_node) return;
    if (parent_node->left == node) {
        parent_node->left = child;
    } else if (parent_node->right == node) {
        parent_node->right = child;
    } else {
        return;
    }
    if (child) {
        child->parent = parent_node;
    }
}

void TreeDelete(Tree* tree, Node* node, AppContext* context) {
    if (!node) return;
    if (node->left) {
        TreeDelete(tree, node->left, context);
    }
    if (node->right) {
        TreeDelete(tree, node->right, context);
    }
    Node** parent_ref = nullptr;
    if (node->parent == nullptr) {
        if (tree->root != node) return;
        parent_ref = &tree->root;
    } else {
        Node* p = node->parent;
        if (p->left == node) {
            parent_ref = &p->left;
        } else if (p->right == node) {
            parent_ref = &p->right;
        } else {
            return;
        }
    }
    ReConnect(parent_ref, node, nullptr);
    RemoveAllOccurrences(node, *context);
    if (tree && tree->size > 0) {
        tree->size--;
    }
    free(node);
}

int CmpDouble(const double a, const double b) {
    const double EPS = 1e-9;
    if (fabs(b - a) < EPS) return 0;
    return (a > b) - (a < b);
}

bool IsZero(const double num) {
    return CmpDouble(num,0.0) == 0;
}

bool ContainsVar(Node* node) {
    if (!node) return false;
    if (node->type == VAR) return true;
    if (node->type == NUM) return false;
    bool left = false, right = false;
    if (node->left)
        left = ContainsVar(node->left);
    if (node->right)
        right = ContainsVar(node->right);
    return (left || right);
}

bool FindVariable(size_t* ind_param, const char variable[MAX_VARIABLE_LEN], char variableNames[MAX_COUNT_VARIABLE][MAX_VARIABLE_LEN]) {
    for (size_t i = 0; i < MAX_COUNT_VARIABLE; i++) {
        if (variableNames[i][0] == '\0') break;
        if (strcmp(variable, variableNames[i]) == 0) {
            *ind_param = i;
            return true;
        }
    }
    return false;
}

long long GetFileSize(FILE* file) {
    if (!file) return 0;
    long long start = ftell(file);
    fseek(file, 0, SEEK_END);
    long long size = ftell(file);
    fseek(file, start, SEEK_SET);
    return size;
}

int SkipSpaces(Text* data, size_t i) {
    while (data->Buf && isspace((unsigned char)data->Buf[i])) i++;
    return (int)i;
}

Tree_type Eval(Node* node, Tree_type var_data) {
    if (!node) return 0;
    if (node->type == NUM)
        return node->data.num;
    if (node->type == VAR)
        return var_data;
    Tree_type left_data = 0, right_data = 0;
    if (node->left)
        left_data = Eval(node->left, var_data);
    if (node->right)
        right_data = Eval(node->right, var_data);
    switch (node->data.op) {
        case ADD: return left_data + right_data;
        case SUB: return left_data - right_data;
        case MULT:return left_data * right_data;
        case DIV: return left_data / right_data;
        case SIN: return sin(right_data);
        case COS: return cos(right_data);
        case POW: return pow(left_data, right_data);
        case LN:  return log(right_data);
        case LG:  return log10(right_data);
        case TG:  return tan(right_data);
        case CTG: return 1.0/tan(right_data);
        default:
            assert(false);
    }
    return 0;
}

Node* CreateNode(Data data, Types type, Node* left, Node* right, AppContext context) {
    Node* new_node = (Node*)calloc(1, sizeof(Node));
    InsertNode(new_node, context);
    if (left)
        left->parent = new_node;
    if (right)
        right->parent = new_node;
    new_node->left  = left;
    new_node->right = right;
    new_node->type  = type;
    if (type == NUM) {
        new_node->data.num = data.num;
    } else if (type == OP) {
        new_node->data.op = data.op;
    } else if (type == VAR) {
        new_node->data.ind_var_arr = data.ind_var_arr;
    }
    new_node->free = false;
    return new_node;
}

static Type_error ParseOperation(Text* data, size_t* i, Operations* type) {
    if (strncmp("sin", data->Buf + *i, 3) == 0) {
        *type = SIN; *i += 3;
    } else if (strncmp("cos", data->Buf + *i, 3) == 0) {
        *type = COS; *i += 3;
    } else if (strncmp("tg", data->Buf + *i, 2) == 0) {
        *type = TG; *i += 2;
    } else if (strncmp("ctg", data->Buf + *i, 3) == 0) {
        *type = CTG; *i += 3;
    } else if (strncmp("ln", data->Buf + *i, 2) == 0) {
        *type = LN; *i += 2;
    } else if (strncmp("lg", data->Buf + *i, 2) == 0) {
        *type = LG; *i += 2;
    } else if (data->Buf[*i] == '+') {
        *type = ADD; (*i)++;
    } else if (data->Buf[*i] == '-') {
        *type = SUB; (*i)++;
    } else if (data->Buf[*i] == '*') {
        *type = MULT; (*i)++;
    } else if (data->Buf[*i] == '/') {
        *type = DIV; (*i)++;
    } else if (data->Buf[*i] == '^') {
        *type = POW; (*i)++;
    } else {
        *type = UKNOWN_TYPE;
    }
    return TREE_OK;
}

void Lexer(Token** tokens, Text* data, char variableNames[MAX_COUNT_VARIABLE][MAX_VARIABLE_LEN]) {
    size_t i = 0;
    size_t count_token = 0;
    i = (size_t)SkipSpaces(data, i);
    size_t count_left_brackets = 0;
    size_t count_right_brackets = 0;
    size_t p = 0;
    while (data->Buf && data->Buf[i] != '\0') {
        i = (size_t)SkipSpaces(data, i);
        if (data->Buf[i] == '(') {
            (*tokens)[count_token].type = OP;
            (*tokens)[count_token].data.op = OPEN_BRACKET;
            i++;
            count_left_brackets++;
        } else if (data->Buf[i] == ')') {
            (*tokens)[count_token].type = OP;
            (*tokens)[count_token].data.op = CLOSE_BRACKET;
            i++;
            count_right_brackets++;
        } else if (isdigit((unsigned char)data->Buf[i])) {
            double val = 0;
            int check = 0;
            sscanf(data->Buf + i, "%lf%n", &val, &check);
            (*tokens)[count_token].data.num = val;
            (*tokens)[count_token].type = NUM;
            i += (size_t)check;
        } else {
            Operations t = UKNOWN_TYPE;
            ParseOperation(data, &i, &t);
            if (t != UKNOWN_TYPE && t != OPEN_BRACKET && t != CLOSE_BRACKET) {
                (*tokens)[count_token].type = OP;
                (*tokens)[count_token].data.op = t;
            } else if (t == UKNOWN_TYPE) {
                (*tokens)[count_token].type = VAR;
                char varName[MAX_VARIABLE_LEN] = {};
                size_t ind = 0;
                while (isalpha((unsigned char)data->Buf[i])) {
                    varName[ind++] = data->Buf[i++];
                }
                varName[ind] = '\0';
                size_t idx_found = (size_t)-1;
                for (size_t idx = 0; idx < p; idx++) {
                    if (strcmp(varName, variableNames[idx]) == 0) {
                        idx_found = idx;
                        break;
                    }
                }
                if (idx_found == (size_t)-1) {
                    strcpy(variableNames[p], varName);
                    (*tokens)[count_token].data.ind_var_arr = p;
                    p++;
                } else {
                    (*tokens)[count_token].data.ind_var_arr = idx_found;
                }
            }
        }
        count_token++;
    }
    if (count_left_brackets != count_right_brackets) {
        printf("Your example is incorrect\n");
        *tokens = nullptr;
    } else {
        (*tokens)[count_token].type = OP;
        (*tokens)[count_token].data.op = END;
    }
}

Node* GetN(Read* read, AppContext context) {
    Data data = {.num = read->str[read->index++].data.num};
    return CreateNode(data, NUM, nullptr, nullptr, context);
}

Node* GetV(Read* read, AppContext context) {
    if (read->str[read->index].type == VAR) {
        size_t old_index = read->index;
        read->index++;
        Data data = {.ind_var_arr = read->str[old_index].data.ind_var_arr};
        return CreateNode(data, VAR, nullptr, nullptr, context);
    } else {
        return GetN(read, context);
    }
}

Node* GetF(Read* read, AppContext context) {
    if (read->str[read->index].type == OP &&
       (read->str[read->index].data.op == SIN || read->str[read->index].data.op == COS ||
        read->str[read->index].data.op == LN  || read->str[read->index].data.op == LG ||
        read->str[read->index].data.op == TG  || read->str[read->index].data.op == CTG)) {
        size_t old_index = read->index;
        read->index++;
        Node* val = GetP(read, context);
        Data data = {.op = read->str[old_index].data.op};
        return CreateNode(data, OP, nullptr, val, context);
    } else {
        return GetV(read, context);
    }
}

Node* GetP(Read* read, AppContext context) {
    if (read->str[read->index].type == OP && read->str[read->index].data.op == OPEN_BRACKET) {
        read->index++;
        Node* val = GetE(read, context);
        read->index++; 
        return val;
    } else {
        return GetF(read, context);
    }
}

Node* GetD(Read* read, AppContext context) {
    Node* val = GetP(read, context);
    if (read->str[read->index].type == OP && read->str[read->index].data.op == POW) {
        read->index++;
        Node* val2 = GetD(read, context);
        Data data = {.op = POW};
        val = CreateNode(data, OP, val, val2, context);
    }
    return val;
}

Node* GetT(Read* read, AppContext context) {
    Node* val = GetD(read, context);
    while (read->str[read->index].type == OP &&
          (read->str[read->index].data.op == MULT || read->str[read->index].data.op == DIV)) {
        size_t old_index = read->index;
        read->index++;
        Node* val2 = GetD(read, context);
        Data data = {.op = read->str[old_index].data.op};
        val = CreateNode(data, OP, val, val2, context);
    }
    return val;
}

Node* GetE(Read* read, AppContext context) {
    Node* val = GetT(read, context);
    while (read->str[read->index].type == OP &&
          (read->str[read->index].data.op == ADD || read->str[read->index].data.op == SUB)) {
        size_t old_index = read->index;
        read->index++;
        Node* val2 = GetT(read, context);
        Data data = {.op = read->str[old_index].data.op};
        val = CreateNode(data, OP, val, val2, context);
    }
    return val;
}

Node* GetG(Read* read, AppContext context) {
    Node* val = GetE(read, context);
    if (read->str[read->index].data.op != END)
        printf("Syntax Error. Index: %lu\n", read->index);
    return val;
}

void FileInput(FILE* file, Text* data);

void TreeRead(Tree* tree, Text* data, Read* read, char variableNames[MAX_COUNT_VARIABLE][MAX_VARIABLE_LEN], AppContext context) {
    Token* tokens = (Token*)calloc(MAX_TOKENS, sizeof(Token));
    Lexer(&tokens, data, variableNames);
    if (tokens) {
        read->str = tokens;
        read->index = 0;
        tree->root = GetG(read, context);
    }
    free(tokens);
}

enum Changes {
    NO_CHANGES  = 0,
    HAS_CHANGES = 1,
};

static void ConvertNodeToNumber(Tree* tree, Node** node_ref, const double num, AppContext* context) {
    Node* n = *node_ref;
    n->data.num = num;
    n->type     = NUM;
    if (n->left) {
        TreeDelete(tree, n->left, context);
        n->left = nullptr;
    }
    if (n->right) {
        TreeDelete(tree, n->right, context);
        n->right = nullptr;
    }
}

static bool OptimizeConst(Tree* tree, Node* node, Changes* change, AppContext* context);
static Tree_type OptimizeDeleteNeutrals(Tree* tree, Node** parent, Node* node, Changes* change, AppContext* context);

static void SaveCurrentTreeToBuffer(Tree* tree, AppContext* context, char* buf) {
    FILE* buf_file = fopen("buf.txt", "w+");
    if (buf_file) {
        DumpNode(tree->root, context->variableNames, buf_file);
        rewind(buf_file);
        fgets(buf, 1000, buf_file);
        fclose(buf_file);
    }
}

static void PrintOptimizationResult(Tree* tree, AppContext* context, const char* buf) {
    FILE* f = GetFileName();
    if (f) {
        fprintf(f, "%s\n", "упростив получим \n");
        fprintf(f, "\\begin{center}$");
        fprintf(f, "%s", buf);
        fprintf(f, " = ");
        DumpNode(tree->root, context->variableNames, STANDART_TEX);
        fprintf(f, "$\\end{center}\\ \n");
    }
}

static Changes PerformOptimizationStep(Tree* tree, AppContext* context, char* buf) {
    Changes change = NO_CHANGES;
    SaveCurrentTreeToBuffer(tree, context, buf);
    OptimizeConst(tree, tree->root, &change, context);
    OptimizeDeleteNeutrals(tree, &tree->root, tree->root, &change, context);
    return change;
}

void Optimize(Tree* tree, AppContext* context, OptimizePrint print) {
    while (true) {
        char buf[MAX_TOKENS] = {};
        Changes change = PerformOptimizationStep(tree, context, buf);

        if (change != NO_CHANGES && print == PRINT_OPTIMIZE) {
            PrintOptimizationResult(tree, context, buf);
        }

        if (change == NO_CHANGES)
            break;
    }
}

static bool OptimizeConst(Tree* tree, Node* node, Changes* change, AppContext* context) {
    if (node->type == NUM)
        return true;
    if (node->type == VAR)
        return false;
    bool left = false, right = false;
    if (node->left)
        left = OptimizeConst(tree, node->left, change, context);
    if (node->right)
        right = OptimizeConst(tree, node->right, change, context);
    if (left && right) {
        double val = Eval(node, 0);
        ConvertNodeToNumber(tree, &node, val, context);
        *change = HAS_CHANGES;
        return true; 
    }
    return false;
}

static Tree_type OptimizeDeleteNeutrals(Tree* tree, Node** parent, Node* node, Changes* change, AppContext* context) {
    if (!node) return NAN;
    if (node->type == NUM)
        return node->data.num;
    if (node->type == VAR)
        return -1;
    Tree_type left_val = NAN;
    Tree_type right_val = NAN;
    if (node->left)
        left_val = OptimizeDeleteNeutrals(tree, &node->left, node->left, change, context);
    if (node->right)
        right_val = OptimizeDeleteNeutrals(tree, &node->right, node->right, change, context);
    if (node->type == OP) {
        switch (node->data.op) {
            case ADD:
                if (!isnan(left_val) && IsZero(left_val)) {
                    ReConnect(parent, node, node->right);
                    *change = HAS_CHANGES;
                    return 0;
                } else if (!isnan(right_val) && IsZero(right_val)) {
                    ReConnect(parent, node, node->left);
                    *change = HAS_CHANGES;
                    return 0;
                }
                break;
            case SUB:
                if (!isnan(right_val) && IsZero(right_val)) {
                    ReConnect(parent, node, node->left);
                    *change = HAS_CHANGES;
                    return left_val;
                }
                break;
            case MULT:
                if ((!isnan(left_val) && IsZero(left_val)) || (!isnan(right_val) && IsZero(right_val))) {
                    ConvertNodeToNumber(tree, parent, 0.0, context);
                    *change = HAS_CHANGES;
                    return 0;
                } else if (!isnan(right_val) && CmpDouble(right_val, 1) == 0) {
                    ReConnect(parent, node, node->left);
                    *change = HAS_CHANGES;
                    return left_val;
                } else if (!isnan(left_val) && CmpDouble(left_val, 1) == 0) {
                    ReConnect(parent, node, node->right);
                    *change = HAS_CHANGES;
                    return right_val;
                }
                break;
            case DIV:
                if (!isnan(left_val) && IsZero(left_val)) {
                    ConvertNodeToNumber(tree, parent, 0.0, context);
                    *change = HAS_CHANGES;
                    return 0;
                } else if (!isnan(right_val) && CmpDouble(right_val, 1) == 0) {
                    ReConnect(parent, node, node->left);
                    *change = HAS_CHANGES;
                    return left_val;
                }
                break;
            case POW:
                if (!isnan(left_val) && IsZero(left_val)) {
                    ConvertNodeToNumber(tree, parent, 0.0, context);
                    *change = HAS_CHANGES;
                    return 0; 
                } else if (!isnan(left_val) && CmpDouble(left_val, 1) == 0) {
                    ConvertNodeToNumber(tree, parent, 1.0, context);
                    *change = HAS_CHANGES;
                    return 1; 
                } else if (!isnan(right_val) && CmpDouble(right_val, 1) == 0) {
                    ReConnect(parent, node, node->left);
                    *change = HAS_CHANGES;
                    return left_val; 
                }
                if (!isnan(left_val) && CmpDouble(right_val, 0) == 0) {
                    ConvertNodeToNumber(tree, parent, 1.0, context);
                    *change = HAS_CHANGES;
                    return 1;
                }
                break;
            default:
                break;
        }
    }
    return -1;
}
