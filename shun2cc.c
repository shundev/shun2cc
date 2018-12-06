#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
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

char *regs[] = {"rdi", "rsi", "r10", "r11", "r12", "r13", "r14", "r15", NULL};

int cur;

char *gen(Node *node) {
    if (node->type == ND_NUM) {
        char *reg = regs[cur++];
        if (!reg) {
            error("Register exhausted");
        }

        printf("  MOV %s, %d\n", reg, node->value);
        return reg;
    }

    char *dst = gen(node->left);
    char *src = gen(node->right);

    switch (node->type) {
        case '+':
            printf("  ADD %s, %s\n", dst, src);
            return dst;
        case '-':
            printf("  SUB %s, %s\n", dst, src);
            return dst;
        default:
            assert(0 && "unknown operator");
            break;
    }
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: 9cc <code>\n");
        return 1;
    }

    tokenize(argv[1]);
    Node *node = expr();

    printf(".intel_syntax noprefix\n");
    printf(".global _main\n");
    printf("_main:\n");
    printf("  MOV rax, %s\n", gen(node));
    printf("  RET\n");
    return 0;
}