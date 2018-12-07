#include "shun2cc.h"

static char *gen_label()
{
    static int n;
    char buf[10];
    sprintf(buf, ".L%d", n++);
    return strdup(buf);
}


void gen_x86(Vector *irv)
{
    char *ret = gen_label();

    printf("  PUSH rbp\n");
    printf("  MOV rbp, rsp\n");

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
                printf("  JMP %s\n", ret);
                break;
            case IR_ALLOCA:
                if (ir->right) {
                    printf("  SUB rsp, %d\n", ir->right);
                }
                printf("  MOV %s, rsp\n", regs[ir->left]);
                break;
            case IR_LOAD:
                printf("  MOV %s, [%s]\n", regs[ir->left], regs[ir->right]);
                break;
            case IR_STORE:
                printf("  MOV [%s], %s\n", regs[ir->left], regs[ir->right]);
                break;
            case '+':
                printf("  ADD %s, %s\n", regs[ir->left], regs[ir->right]);
                break;
            case '-':
                printf("  SUB %s, %s\n", regs[ir->left], regs[ir->right]);
                break;
            case '*':
                printf("  MOV rax, %s\n", regs[ir->right]);
                printf("  MUL %s\n", regs[ir->left]);
                printf("  MOV %s, rax\n", regs[ir->left]);
                break;
            case '/':
                printf("  MOV rax, %s\n", regs[ir->left]);
                // とりあえず除算の前に実行するらしい。符号拡張とは？
                printf("  CQO\n");
                printf("  DIV %s\n", regs[ir->right]);
                printf("  MOV %s, rax\n", regs[ir->left]);
                break;
            case IR_NOP:
                break;
            default:
                assert(0 && "unknown operator");
        }
    }

    printf("%s:\n", ret);
    printf("  MOV rsp, rbp\n");
    printf("  MOV rsp, rbp\n");
    printf("  POP rbp\n");
    printf("  RET\n");
}
