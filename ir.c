#include "shun2cc.h"

static IR *new_ir(int op, int left, int right)
{
    IR *ir = malloc(sizeof(IR));
    ir->op = op;
    ir->left = left;
    ir->right = right;
    return ir;
}

static int gen(Vector *v, Node *node)
{
    static int regno;

    if (node->type == ND_NUM) {
        int r = regno++;
        vec_push(v, new_ir(IR_IMM, r, node->value));
        return r;
    }

    assert(node->type == '+' || node->type == '-');

    int left = gen(v, node->left);
    int right = gen(v, node->right);

    vec_push(v, new_ir(node->type, left, right));
    vec_push(v, new_ir(IR_KILL, right, 0));
    return left;
}

Vector *gen_ir(Node *node)
{
    Vector *v = new_vec();
    int r = gen(v, node);
    vec_push(v, new_ir(IR_RETURN, r, 0));
    return v;
}
