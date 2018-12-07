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
}
