#pragma once

#include <cstddef>

#define MAX_VARIABLE_LEN 10
#define MAX_COUNT_VARIABLE 20
#define MAX_TOKENS 1000
#define MAX_COMMAND_LENGTH 125
#define MAX_OP_LEN 10

enum Operations {
    UKNOWN_TYPE,
    ADD,
    SUB,
    DIV,
    MULT,
    COS,
    SIN,
    POW,
    TG,
    CTG,
    LN,
    LG,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    END
};

enum Types {
    NO_TYPE,
    OP,
    NUM,
    VAR,
};

union Data {
    double num;
    Operations op;
    size_t ind_var_arr;
};

struct Token {
    Types type;
    Data data;
    int err;
};

inline Data CreateData(Operations op) {
    Data data = {};
    data.op = op;
    return data;
}

#define D_ADD(L, R)   CreateNode(CreateData(ADD), OP, L, R, *context)
#define D_SUB(L, R)   CreateNode(CreateData(SUB), OP, L, R, *context)
#define D_MULT(L, R)  CreateNode(CreateData(MULT), OP, L, R, *context)
#define D_DIV(L, R)   CreateNode(CreateData(DIV), OP, L, R, *context)
#define D_SIN(L, R)   CreateNode(CreateData(SIN), OP, L, R, *context)
#define D_COS(L, R)   CreateNode(CreateData(COS), OP, L, R, *context)
#define D_LN(L, R)    CreateNode(CreateData(LN),  OP, L, R, *context)
#define D_POW(L, R)   CreateNode(CreateData(POW), OP, L, R, *context)
