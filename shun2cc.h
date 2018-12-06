#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>

noreturn void error(char *fmt, ...);

typedef struct {
    void **data;
    int capacity;
    int len;
} Vector;

Vector *new_vec(void);
void vec_push(Vector *v, void *elem);

enum {
    TK_NUM = 256,
    TK_EOF,
};

typedef struct {
    int type;
    int value;
    char *asString;
} Token;

Vector *tokenize(char *p);

enum {
    ND_NUM = 256,
};

typedef struct Node {
    int type;
    struct Node *left;
    struct Node *right;
    int value;
} Node;

Node *parse(Vector *tokens);

enum {
    IR_IMM,
    IR_MOV,
    IR_RETURN,
    IR_KILL,
    IR_NOP,
};

typedef struct {
    int op;
    int left;
    int right;
} IR;

Vector *gen_ir(Node *node);

extern char *regs[];

void alloc_regs(Vector *irv);

void gen_x86(Vector *irv);
