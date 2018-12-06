#include "shun2cc.h"

static Vector *tokens;
static int pos;

Node *new_node(int op, Node *left, Node *right)
{
    Node *node = malloc(sizeof(Node));
    node->type = op;
    node->left = left;
    node->right = right;
    return node;
}

Node *new_node_num(int value)
{
    Node *node = malloc(sizeof(Node));
    node->type = ND_NUM;
    node->value = value;
    return node;
}

Node *number()
{
    Token *t = tokens->data[pos];
    if (t->type != TK_NUM) {
        error("Number expected, but got %s", t->asString);
        return NULL;
    }

    pos++;
    return new_node_num(t->value);
}

Node *expr()
{
    Node *left = number();
    for (;;) {
        Token *t = tokens->data[pos];
        int op = t->type;
        if (op != '+' && op != '-') {
            break;
        }

        pos++;
        left = new_node(op, left, number());
    }

    Token *t = tokens->data[pos];
    if (t->type != TK_EOF) {
        error("stray token: %s %d %d", t->asString, t->type, t->value);
    }

    return left;
}

Node *parse(Vector *v) {
    tokens = v;
    pos = 0;
    return expr();
}
