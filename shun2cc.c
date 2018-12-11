#include "shun2cc.h"

int main(int argc, char **argv)
{
    if (!strcmp(argv[1], "-test")) {
        util_test();
        return 0;
    }

    char *input;
    bool dump_ir1 = false;
    bool dump_ir2 = false;

    if (argc == 3 && !strcmp(argv[1], "-dump-ir1")) {
        dump_ir1 = true;
        input = argv[2];
    } else if (argc == 3 && !strcmp(argv[1], "-dump-ir2")) {
        dump_ir2 = true;
        input = argv[2];
    } else {
        if (argc != 2) {
            error("Usage: shun2cc [-test] [-dump-ir1] [-dump-ir2] <code>\n");
        }

        input = argv[1];
    }

    Vector *tokens = tokenize(input);
    Node *node = parse(tokens);

    Vector *irv = gen_ir(node);
    if (dump_ir1) {
        dump_ir(irv);
    }

    alloc_regs(irv);

    if (dump_ir2) {
        dump_ir(irv);
    }

    printf(".intel_syntax noprefix\n");
    printf(".global _main\n");
    printf("_main:\n");
    gen_x86(irv);

    return 0;
}