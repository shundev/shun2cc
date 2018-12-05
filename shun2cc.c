#include <ctype.h>
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

void fail(int i) {
    fprintf(stderr, "Unexpected %dth token: %s\n", i, tokens[i].asString);
    exit(1);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: 9cc <code>\n");
        return 1;
    }

    tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".global _main\n");
    printf("_main:\n");

    if (tokens[0].type != TK_NUM) {
        fail(0);
    }

    printf("  MOV rax, %d\n", tokens[0].value);

    int i = 1;
    while (tokens[i].type != TK_EOF) {
        if (tokens[i].type == '+') {
            i++;
            if (tokens[i].type != TK_NUM) {
                fail(i);
            }

            printf("  ADD rax, %d\n", tokens[i].value);
            i++;
            continue;
        }

        if (tokens[i].type == '-') {
            i++;
            if (tokens[i].type != TK_NUM) {
                fail(i);
            }

            printf("  SUB rax, %d\n", tokens[i].value);
            i++;
            continue;
        }

        fail(i);
    }

    printf("  RET\n");
    return 0;
}