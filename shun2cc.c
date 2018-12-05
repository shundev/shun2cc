#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: 9cc <code>\n");
        return 1;
    }

    char *p = argv[1];

    printf(".intel_syntax noprefix\n");
    printf(".global _main\n");
    printf("_main:\n");

    // strtol moves p by 1 after reading
    printf("  MOV rax, %ld\n", strtol(p, &p, 10));

    while (*p) {
        if (*p == '+') {
            p++; // Skip +
            printf("  ADD rax, %ld\n", strtol(p, &p, 10));
            continue;
        }

        if (*p == '-') {
            p++; // Skip -
            printf("  SUB rax, %ld\n", strtol(p, &p, 10));
            continue;
        }

        fprintf(stderr, "Unexpected character: %c\n", *p);
        return 1;
    }

    printf("  RET\n");
    return 0;
}