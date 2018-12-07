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

static bool consume(int type)
{
    Token *t = tokens->data[pos];
    if (t->type != type) {
        return false;
    }

    pos++;
    return true;
}

static Node *new_node(int op, Node *left, Node *right)
{
    Node *node = malloc(sizeof(Node));
    node->type = op;
    node->left = left;
    node->right = right;
    return node;
}

static Node *term()
{
    Node *node = malloc(sizeof(Node));
    Token *t = tokens->data[pos++];

    if (t->type == TK_NUM) {
        node->type = ND_NUM;
        node->value = t->value;
        return node;
    }

    if (t->type == TK_IDENT) {
        node->type = ND_IDENT;
        node->name = t->name;
        return node;
    }

    error("Number expected, but got %s", t->asString);
}


static Node *mul()
{
    Node *left = term();
    for (;;) {
        Token *t = tokens->data[pos];
        int op = t->type;
        if (op != '*' && op != '/') {
            return left;
        }

        pos++;
        left = new_node(op, left, term());
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


static Node *assign()
{
    Node *left = expr();
    if (consume('=')) {
        return new_node('=', left, expr());
    }

    return left;
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
            e->expr = assign();
        } else {
            e->type = ND_EXPR_STMT;
            e->expr = assign();
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
