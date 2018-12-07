#include "shun2cc.h"

static Vector *code;

static IR *add(int op, int left, int right)
{
    IR *ir = malloc(sizeof(IR));
    ir->op = op;
    ir->left = left;
    ir->right = right;
    vec_push(code, ir);
    return ir;
}

static int gen_expr(Node *node)
{
    static int regno;

    if (node->type == ND_NUM) {
        int r = regno++;
        add(IR_IMM, r, node->value);
        return r;
    }

    assert(strchr("+-*/", node->type));

    int left = gen_expr(node->left);
    int right = gen_expr(node->right);

    add(node->type, left, right);
    add(IR_KILL, right, 0);
    return left;
}

static void gen_stmt(Node *node)
{
    if (node->type == ND_RETURN) {
        int r = gen_expr(node->expr);
        add(IR_RETURN, r, 0);
        add(IR_KILL, r, 0);
        return;
    }

    if (node->type == ND_EXPR_STMT) {
        int r = gen_expr(node->expr);
        add(IR_KILL, r, 0);
        return;
    }

    if (node->type == ND_COMP_STMT) {
        for (int i=0; i<node->stmts->len; i++) {
            gen_stmt(node->stmts->data[i]);
        }

        return;
    }

    error("unknown node: %d", node->type);
}

Vector *gen_ir(Node *node)
{
    assert(node->type == ND_COMP_STMT);
    code = new_vec();
    gen_stmt(node);
    return code;
}
