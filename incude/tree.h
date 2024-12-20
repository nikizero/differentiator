#pragma once
#include "../incude/dsl.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cctype>
#include <cmath>

struct AppContext;

struct Node {
    Node* left;
    Node* right;
    Node* parent;
    Data data;
    Types type;
    bool free;
};

struct Tree {
    Node* root;
    size_t size;
    int status;
};

struct Text {
    char* Buf;
    long long BufSize;
};

struct Read {
    size_t index;
    Token* str;
};

struct TREE_STATUS {
    int CodeError;
    const char* NameError;
};

struct Operation {
    Operations op_name;
    char op_char_name[MAX_OP_LEN];
};

enum Tree_status {
    TREE_OK = 0,
    TREE_IS_NULL = 1 << 0,
    TREE_ROOT_IS_NULL = 1 << 1,
    NODE_PTR_IS_NULL = 1 << 2,
    TREE_CANT_HAVE_THIS_CHILD = 1 << 3,
    TREE_SIZE_LESS_THAN_ZERO = 1 << 4,
    TREE_LINKING_ERROR = 1 << 5,
    NUMBER_OF_ERROR = 6
};

enum OptimizePrint {
    NO_PRINT_OPTIMIZE  = 0,
    PRINT_OPTIMIZE     = 1,
};

#define STANDART_TEX  nullptr

const TREE_STATUS ErrorMas[] = {
    {TREE_OK,                     "NO ERROR"},
    {TREE_IS_NULL,                "TREE_IS_NULL"},
    {TREE_ROOT_IS_NULL,           "TREE_ROOT_IS_NULL"},
    {NODE_PTR_IS_NULL,            "NODE_PTR_IS_NULL"},
    {TREE_CANT_HAVE_THIS_CHILD,   "TREE_CANT_HAVE_THIS_CHILD"},
    {TREE_SIZE_LESS_THAN_ZERO,    "TREE_SIZE_LESS_THAN_ZERO"},
    {TREE_LINKING_ERROR,          "TREE_LINKING_ERROR"}
};

const Operation OperationArray[] = {
    {ADD,  "+"},
    {SUB,  "-"},
    {DIV,  "/"},
    {MULT, "*"},
    {COS,  "cos"},
    {SIN,  "sin"},
    {POW,  "^"},
    {TG,   "tg"},
    {CTG,  "ctg"},
    {LN,   "ln"},
    {LG,   "lg"}
};

typedef double Tree_type;

Tree_type Eval(Node* node, Tree_type var_data);
void TreeCtor(Tree* tree);
int TreeDtor(Tree* tree, AppContext* context);
Node* CreateNode(Data data, Types type, Node* left, Node* right, AppContext context);
void TreeDelete(Tree* tree, Node* node, AppContext* context);
bool ContainsVar(Node* node);
bool FindVariable(size_t* ind_param, const char variable[MAX_VARIABLE_LEN], char variableNames[MAX_COUNT_VARIABLE][MAX_VARIABLE_LEN]);

int SkipSpaces(Text* data, size_t i);
int CmpDouble(const double a, const double b);
bool IsZero(const double num);
long long GetFileSize(FILE* file);

void Lexer(Token** tokens, Text* data, char variableNames[MAX_COUNT_VARIABLE][MAX_VARIABLE_LEN]);

Node* GetG(Read* read, AppContext context); // Обрабатывает все выражение.
Node* GetE(Read* read, AppContext context); // Обрабатывает операции сложения и вычитания (+, -).
Node* GetT(Read* read, AppContext context); // Обрабатывает операции умножения и деления (*, /).
Node* GetD(Read* read, AppContext context); // Обрабатывает степени (^).
Node* GetP(Read* read, AppContext context); // Обрабатывает выражения в скобках или вызовы функций.
Node* GetF(Read* read, AppContext context); // Обрабатывает математические функции (sin, cos, ln, tg и т.д.) или переменные.
Node* GetV(Read* read, AppContext context); // Обрабатывает переменные.
Node* GetN(Read* read, AppContext context); // Обрабатывает числа.



void TreeDumpFunction(AppContext* context, Node* node, const char* path, const char* signature, unsigned line);
#define TREE_DUMP(context, tree) TreeDumpFunction((context), tree.root, __FILE__, __PRETTY_FUNCTION__, __LINE__)

void TreeRead(Tree* tree, Text* data, Read* read, char variableNames[MAX_COUNT_VARIABLE][MAX_VARIABLE_LEN], AppContext context);
void Optimize(Tree* tree, AppContext* context, OptimizePrint print);
typedef int Type_error;

void FreeNodes(AppContext context);
void InsertNode(Node* node, AppContext context);
void RemoveAllOccurrences(Node* node, AppContext context);
void ReConnect(Node** parent_ref, Node* node, Node* child);