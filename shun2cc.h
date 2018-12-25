#define _GNU_SOURCE
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>

noreturn void error(char *fmt, ...);
char *format(char *fmt, ...);

typedef struct {
    void **data;
    int capacity;
    int len;
} Vector;

Vector *new_vec(void);
void vec_push(Vector *v, void *elem);

typedef struct {
    Vector *keys;
    Vector *vals;
} Map;

Map *new_map(void);
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);
bool map_exists(Map *map, char *key);

void util_test();

enum {
    TK_NUM = 256,
    TK_IDENT,
    TK_IF,
    TK_ELSE,
    TK_RETURN,
    TK_EOF,
};

typedef struct {
    int type;
    int value;
    char *name;
    char *asString;
} Token;

Vector *tokenize(char *p);

enum {
    ND_NUM = 256,
    ND_IDENT,
    ND_IF,
    ND_RETURN,
    ND_COMP_STMT,
    ND_EXPR_STMT,
};

typedef struct Node {
    int type;
    struct Node *left;
    struct Node *right;
    int value;
    char *name;
    struct Node *expr;
    Vector *stmts;
    struct Node *cond;
    struct Node *then;
    struct Node *els;
} Node;

Node *parse(Vector *tokens);

enum {
    IR_IMM = 256,
    IR_ADD_IMM,
    IR_MOV,
    IR_RETURN,
    IR_LABEL,
    IR_JMP,
    IR_UNLESS,
    IR_ALLOCA,
    IR_LOAD,
    IR_STORE,
    IR_KILL,
    IR_NOP,
};

typedef struct {
    int op;
    int left;
    int right;
} IR;

enum {
    IR_TY_NOARG,
    IR_TY_REG,
    IR_TY_LABEL,
    IR_TY_REG_REG,
    IR_TY_REG_IMM,
    IR_TY_REG_LABEL,
};

typedef struct {
    int op;
    char *name;
    int type;
} IRInfo;

extern IRInfo irinfo[];
IRInfo *get_irinfo(IR *ir);

Vector *gen_ir(Node *node);
void dump_ir(Vector *);

extern char *regs[];

void alloc_regs(Vector *irv);

void gen_x86(Vector *irv);

char **argv;