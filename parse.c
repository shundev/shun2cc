#include "shun2cc.h"

static Vector *tokens;
static int pos;

static void expect(int type)
{
    Token *t = tokens->data[pos];
    if (t->type != type) {
        error(
            "%c (%d) expected, but got %c (%d)",
            type, type,
            t->type, t->type
        );
    }

    pos++;
}

static Node *new_node(int op, Node *left, Node *right)
{
    Node *node = malloc(sizeof(Node));
    node->type = op;
    node->left = left;
    node->right = right;
    return node;
}

static Node *number()
{
    Token *t = tokens->data[pos];
    if (t->type != TK_NUM) {
        error("Number expected, but got %s", t->asString);
        return NULL;
    }

    pos++;

    Node *node = malloc(sizeof(Node));
    node->type = ND_NUM;
    node->value = t->value;
    return node;
}


static Node *mul()
{
    Node *left = number();
    for (;;) {
        Token *t = tokens->data[pos];
        int op = t->type;
        if (op != '*' && op != '/') {
            return left;
        }

        pos++;
        left = new_node(op, left, number());
    }
}


static Node *expr()
{
    Node *left = mul();
    for (;;) {
        Token *t = tokens->data[pos];
        int op = t->type;
        if (op != '+' && op != '-') {
            return left;
        }

        pos++;
        left = new_node(op, left, mul());
    }
}


static Node *stmt()
{
    Node *node = malloc(sizeof(Node));
    node->type = ND_COMP_STMT;
    node->stmts = new_vec();

    for (;;) {
        Token *t = tokens->data[pos];
        if (t->type == TK_EOF) {
            return node;
        }

        Node *e = malloc(sizeof(Node));

        if (t->type == TK_RETURN) {
            pos++;
            e->type = ND_RETURN;
            e->expr = expr();
        } else {
            e->type = ND_EXPR_STMT;
            e->expr = expr();
        }

        vec_push(node->stmts, e);
        expect(';');
    }
}


Node *parse(Vector *v) {
    tokens = v;
    pos = 0;

    return stmt();
}
