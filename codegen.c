#include "shun2cc.h"


void gen_x86(Vector *irv)
{
    char *ret = ".Lend";

    printf("  PUSH rbp\n");
    printf("  MOV rbp, rsp\n");

    for (int i=0; i<irv->len; i++) {
        IR *ir = irv->data[i];

        switch (ir->op) {
            case IR_IMM:
                printf("  MOV %s, %d\n", regs[ir->left], ir->right);
                break;
            case IR_ADD_IMM:
                printf("  ADD %s, %d\n", regs[ir->left], ir->right);
                break;
            case IR_MOV:
                printf("  MOV %s, %s\n", regs[ir->left], regs[ir->right]);
                break;
            case IR_RETURN:
                printf("  MOV rax, %s\n", regs[ir->left]);
                printf("  JMP %s\n", ret);
                break;
            case IR_CALL: {
                printf("  PUSH rbx\n");
                printf("  PUSH rbp\n");
                printf("  PUSH rsp\n");
                printf("  PUSH r12\n");
                printf("  PUSH r13\n");
                printf("  PUSH r14\n");
                printf("  PUSH r15\n");

                char *arg[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
                for (int i=0; i<ir->nargs; i++) {
                    printf("  MOV %s, %s\n", arg[i], regs[ir->args[i]]);
                }

                printf("  MOV rax, 0\n");
                printf("  CALL _%s\n", ir->name);
                printf("  MOV %s, rax\n", regs[ir->left]);

                printf("  PUSH r15\n");
                printf("  PUSH r14\n");
                printf("  PUSH r13\n");
                printf("  PUSH r12\n");
                printf("  PUSH rsp\n");
                printf("  PUSH rbp\n");
                printf("  PUSH rbx\n");
            }
            case IR_LABEL:
                printf(".L%d:\n", ir->left);
                break;
            case IR_JMP:
                printf("  JMP .L%d\n", ir->left);
                break;
            case IR_UNLESS:
                printf("  CMP %s, 0\n", regs[ir->left]);
                printf("  JE .L%d\n", ir->right);
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
    printf("  POP rbp\n");
    printf("  RET\n");
}
