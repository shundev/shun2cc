#include "shun2cc.h"

char *regs[] = {"rdi", "rsi", "r10", "r11", "r12", "r13", "r14", "r15", NULL};
static bool used[sizeof(regs) / sizeof(*regs)];
static int *reg_map;

static int alloc(int ir_reg)
{
    if (reg_map[ir_reg] != -1) {
        int r = reg_map[ir_reg];
        assert(used[r]);
        return r;
    }

    for (int i=0; i<sizeof(regs) / sizeof(*regs); i++) {
        if (used[i]) {
            continue;
        }
        used[i] = true;
        reg_map[ir_reg] = i;
        return i;
    }

    error("Register exhausted.");
}

static void kill(int r) {
    assert(used[r]);
    used[r] = false;
}

void alloc_regs(Vector *irv)
{
    reg_map = malloc(sizeof(int) * irv->len);
    for (int i=0; i<irv->len; i++) {
        reg_map[i] = -1;
    }

    for (int i=0; i<irv->len; i++) {
        IR *ir = irv->data[i];

        switch (ir->op) {
            case IR_IMM:
            case IR_ALLOCA:
            case IR_RETURN:
                ir->left = alloc(ir->left);
                break;
            case IR_MOV:
            case IR_LOAD:
            case IR_STORE:
            case '+':
            case '-':
            case '*':
            case '/':
                ir->left = alloc(ir->left);
                ir->right = alloc(ir->right);
                break;
            case IR_KILL:
                kill(reg_map[ir->left]);
                ir->op = IR_NOP;
                break;
            default:
                assert(0 && "unknown operator");
        }
    }
}