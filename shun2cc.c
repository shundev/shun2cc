#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>

typedef struct {
    void **data;
    int capacity;
    int len;
} Vector;

Vector *new_vec()
{
    Vector *v = malloc(sizeof(Vector));
    // The size of a void pointer depends on platform.
    v->data = malloc(sizeof(void *) * 16);
    v->capacity = 16;
    v->len = 0;
    return v;
}

void vec_push(Vector *v, void *elem)
{
    if (v->len == v->capacity) {
        v->capacity *= 2;
        v->data = realloc(v->data, sizeof(void *) * v->capacity);
    }

    v->data[v->len++] = elem;
}

enum {
    TK_NUM = 256,
    TK_EOF,
};

typedef struct {
    int type;
    int value;
    char *asString;
} Token;

Token *add_token(Vector *v, int type, char *asString)
{
    Token *t = malloc(sizeof(Token));
    t->type = type;
    t->asString = asString;
    vec_push(v, t);
    return t;
}

Vector *tokenize(char *p)
{
    Vector *v = new_vec();

    int i = 0;
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            add_token(v, *p, p);
            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            Token *t = add_token(v, TK_NUM, p);
            t->value = strtol(p, &p, 10);
            i++;
            continue;
        }

        fprintf(stderr, "Cannot tokenize: %s", p);
        exit(1);
    }

    add_token(v, TK_EOF, p);
    return v;
}

int pos = 0;

enum {
    ND_NUM = 256,
};

typedef struct Node {
    int type;
    struct Node *left;
    struct Node *right;
    int value;
} Node;

Vector *tokens;
int pos;

Node *new_node(int op, Node *left, Node *right)
{
    Node *node = malloc(sizeof(Node));
    node->type = op;
    node->left = left;
    node->right = right;
    return node;
}

Node *new_node_num(int value)
{
    Node *node = malloc(sizeof(Node));
    node->type = ND_NUM;
    node->value = value;
    return node;
}

noreturn void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

Node *number()
{
    Token *t = tokens->data[pos];
    if (t->type != TK_NUM) {
        error("Number expected, but got %s", t->asString);
        return NULL;
    }

    pos++;
    return new_node_num(t->value);
}

Node *expr()
{
    Node *left = number();
    for (;;) {
        Token *t = tokens->data[pos];
        int op = t->type;
        if (op != '+' && op != '-') {
            break;
        }

        pos++;
        left = new_node(op, left, number());
    }

    Token *t = tokens->data[pos];
    if (t->type != TK_EOF) {
        error("stray token: %s %d %d", t->asString, t->type, t->value);
    }

    return left;
}

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

IR *new_ir(int op, int left, int right)
{
    IR *ir = malloc(sizeof(IR));
    ir->op = op;
    ir->left = left;
    ir->right = right;
    return ir;
}


int gen_ir_sub(Vector *v, Node *node)
{
    static int regno;

    if (node->type == ND_NUM) {
        int r = regno++;
        vec_push(v, new_ir(IR_IMM, r, node->value));
        return r;
    }

    assert(node->type == '+' || node->type == '-');

    int left = gen_ir_sub(v, node->left);
    int right = gen_ir_sub(v, node->right);

    vec_push(v, new_ir(node->type, left, right));
    vec_push(v, new_ir(IR_KILL, right, 0));
    return left;
}

Vector *gen_ir(Node *node)
{
    Vector *v = new_vec();
    int r = gen_ir_sub(v, node);
    vec_push(v, new_ir(IR_RETURN, r, 0));
    return v;
}

char *regs[] = {"rdi", "rsi", "r10", "r11", "r12", "r13", "r14", "r15", NULL};
bool used[sizeof(regs) / sizeof(*regs)];
int *reg_map;

int alloc(int ir_reg)
{
    if (reg_map[ir_reg] != -1) {
        int r = reg_map[ir_reg];
        assert(used[r]);
        return r;
    }

    for (int i=0; i<sizeof(regs) / sizeof(*regs); i++) {
        if (used[i]) {
            continue;
        }
        used[i] = true;
        reg_map[ir_reg] = i;
        return i;
    }

    error("Register exhausted.");
}

void kill(int r) {
    assert(used[r]);
    used[r] = false;
}

void alloc_regs(Vector *irv)
{
    reg_map = malloc(sizeof(int) * irv->len);
    for (int i=0; i<irv->len; i++) {
        reg_map[i] = -1;
    }

    for (int i=0; i<irv->len; i++) {
        IR *ir = irv->data[i];

        switch (ir->op) {
            case IR_IMM:
                ir->left = alloc(ir->left);
                break;
            case IR_MOV:
            case '+':
            case '-':
                ir->left = alloc(ir->left);
                ir->right = alloc(ir->right);
                break;
            case IR_RETURN:
                kill(reg_map[ir->left]);
                break;
            case IR_KILL:
                kill(reg_map[ir->left]);
                ir->op = IR_NOP;
                break;
            default:
                assert(0 && "unknown operator");
        }
    }
}

void gen_x86(Vector *irv)
{
    for (int i=0; i<irv->len; i++) {
        IR *ir = irv->data[i];

        switch (ir->op) {
            case IR_IMM:
                printf("  MOV %s, %d\n", regs[ir->left], ir->right);
                break;
            case IR_MOV:
                printf("  MOV %s, %s\n", regs[ir->left], regs[ir->right]);
                break;
            case IR_RETURN:
                printf("  MOV rax, %s\n", regs[ir->left]);
                printf("  RET\n");
                break;
            case '+':
                printf("  ADD %s, %s\n", regs[ir->left], regs[ir->right]);
                break;
            case '-':
                printf("  SUB %s, %s\n", regs[ir->left], regs[ir->right]);
                break;
            case IR_NOP:
                break;
            default:
                assert(0 && "unknown operator");
        }
    }
}


int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: 9cc <code>\n");
        return 1;
    }

    tokens = tokenize(argv[1]);
    for (int i=0; i<tokens->len; i++) {
        Token *t = tokens->data[i];
    }

    Node *node = expr();

    Vector *irv = gen_ir(node);
    alloc_regs(irv);

    printf(".intel_syntax noprefix\n");
    printf(".global _main\n");
    printf("_main:\n");
    gen_x86(irv);

    return 0;
}