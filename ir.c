#include "shun2cc.h"

static Vector *code;
static int regno;
static int basereg;

static Map *vars;
static int bpoff;


static IR *add(int op, int left, int right)
{
    // 値を初期化しないとhas_immが非0になる可能性がある
    IR *ir = calloc(1, sizeof(IR));
    ir->op = op;
    ir->left = left;
    ir->right = right;
    vec_push(code, ir);
    return ir;
}


static int gen_lval(Node *node)
{
    if (node->type != ND_IDENT) {
        error("not an lvalue");
    }

    if (!map_exists(vars, node->name)) {
        map_put(vars, node->name, (void *)(intptr_t)bpoff);
        bpoff += 8;
    }

    int r = regno++;
    int off = (intptr_t)map_get(vars, node->name);
    add(IR_MOV, r, basereg);
    add(IR_ADD_IMM, r, off);
    return r;
}

static int gen_expr(Node *node)
{
    if (node->type == ND_NUM) {
        int r = regno++;
        add(IR_IMM, r, node->value);
        return r;
    }

    if (node->type == ND_IDENT) {
        int r = gen_lval(node);
        add(IR_LOAD, r, r);
        return r;
    }

    if (node->type == '=') {
        int right = gen_expr(node->right);
        int left = gen_lval(node->left);
        add(IR_STORE, left, right);
        add(IR_KILL, right, -1);
        return left;
    }

    assert(strchr("+-*/", node->type));

    int left = gen_expr(node->left);
    int right = gen_expr(node->right);

    add(node->type, left, right);
    add(IR_KILL, right, -1);
    return left;
}

static void gen_stmt(Node *node)
{
    if (node->type == ND_RETURN) {
        int r = gen_expr(node->expr);
        add(IR_RETURN, r, -1);
        add(IR_KILL, r, -1);
        return;
    }

    if (node->type == ND_EXPR_STMT) {
        int r = gen_expr(node->expr);
        add(IR_KILL, r, -1);
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
    regno = 1;
    basereg = 0;
    vars = new_map();
    bpoff = 0;

    IR *alloca = add(IR_ALLOCA, basereg, -1);
    gen_stmt(node);
    alloca->right =bpoff;
    add(IR_KILL, basereg, -1);
    return code;
}

void dump_ir(Vector *irv)
{
    for (int i=0; i<irv->len; i++) {
        IR *ir = irv->data[i];

        switch (ir->op) {
            case IR_IMM:
                printf("IMM\t");
                break;
            case IR_ADD_IMM:
                printf("ADD_IMM\t");
                break;
            case IR_ALLOCA:
                printf("ALLOCA\t");
                break;
            case IR_RETURN:
                printf("RETURN\t");
                break;
            case IR_MOV:
                printf("MOV\t");
                break;
            case IR_LOAD:
                printf("LOAD\t");
                break;
            case IR_STORE:
                printf("STORE\t");
                break;
            case '+':
                printf("+\t");
                break;
            case '-':
                printf("-\t");
                break;
            case '*':
                printf("*\t");
                break;
            case '/':
                printf("/\t");
                break;
            case IR_KILL:
                printf("KILL\t");
                break;
            case IR_NOP:
                printf("NOP\t");
                break;
            default:
                assert(0 && "unknown operator");
        }

        printf(" left = %d\tright = %d\n", ir->left, ir->right);
    }
}
