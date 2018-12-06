#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    TK_NUM = 256,
    TK_EOF,
};

typedef struct {
    int type;
    int value;
    char *asString;
} Token;

Token tokens[100];

void tokenize(char *p)
{
    int i = 0;
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            tokens[i].type = *p;
            tokens[i].asString = p;
            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            tokens[i].type = TK_NUM;
            tokens[i].asString = p;
            tokens[i].value = strtol(p, &p, 10);
            i++;
            continue;
        }

        fprintf(stderr, "Cannot tokenize: %s", p);
        exit(1);
    }

    tokens[i].type = TK_EOF;
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

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

Node *number()
{
    if (tokens[pos].type == TK_NUM) {
        return new_node_num(tokens[pos++].value);
    }

    error("Number expected, but got %s", tokens[pos].asString);
    return NULL;
}

Node *expr()
{
    Node *left = number();
    for (;;) {
        int op = tokens[pos].type;
        if (op != '+' && op != '-') {
            break;
        }

        pos++;
        left = new_node(op, left, number());
    }

    if (tokens[pos].type != TK_EOF) {
        error("stray token: %s", tokens[pos].asString);
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

IR *ins[1000];
int inp;
int regno;

int gen_ir_sub(Node *node)
{
    if (node->type == ND_NUM) {
        int r = regno++;
        ins[inp++] = new_ir(IR_IMM, r, node->value);
        return r;
    }

    assert(node->type == '+' || node->type == '-');

    int left = gen_ir_sub(node->left);
    int right = gen_ir_sub(node->right);

    ins[inp++] = new_ir(node->type, left, right);
    ins[inp++] = new_ir(IR_KILL, right, 0);
    return left;
}

int gen_ir(Node *node)
{
    int r = gen_ir_sub(node);
    ins[inp++] = new_ir(IR_RETURN, r, 0);
}

char *regs[] = {"rdi", "rsi", "r10", "r11", "r12", "r13", "r14", "r15", NULL};
bool used[8];
int reg_map[1000];

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

void alloc_regs()
{
    for (int i=0; i<inp; i++) {
        IR *ir = ins[i];

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

void gen_x86()
{
    for (int i=0; i<inp; i++) {
        IR *ir = ins[i];

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

    for (int i=0; i<sizeof(reg_map) / sizeof(*reg_map); i++) {
        reg_map[i] = -1;
    }

    tokenize(argv[1]);
    Node *node = expr();

    gen_ir(node);
    alloc_regs();

    printf(".intel_syntax noprefix\n");
    printf(".global _main\n");
    printf("_main:\n");
    gen_x86();

    return 0;
}