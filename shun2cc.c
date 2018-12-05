#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: 9cc <code>\n");
        return 1;
    }

    printf(".intel_syntax noprefix\n");
    printf(".global _main\n");
    printf("_main:\n");
    printf("  MOV rax, %d\n", atoi(argv[1]));
    printf("  RET\n");
    return 0;
}