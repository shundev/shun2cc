#include "shun2cc.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: 9cc <code>\n");
        return 1;
    }

    if (!strcmp(argv[1], "-test")) {
        util_test();
        return 0;
    }

    Vector *tokens = tokenize(argv[1]);
    Node *node = parse(tokens);

    Vector *irv = gen_ir(node);
    alloc_regs(irv);

    printf(".intel_syntax noprefix\n");
    printf(".global _main\n");
    printf("_main:\n");
    gen_x86(irv);

    return 0;
}