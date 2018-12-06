#include "shun2cc.h"

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
