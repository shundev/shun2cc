#include "shun2cc.h"

static Token *add_token(Vector *v, int type, char *asString)
{
    Token *t = malloc(sizeof(Token));
    t->type = type;
    t->asString = asString;
    vec_push(v, t);
    return t;
}

Vector *tokenize(char *p)
{
    Vector *v = new_vec();

    int i = 0;
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (strchr("+-*", *p)) {
            add_token(v, *p, p);
            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            Token *t = add_token(v, TK_NUM, p);
            t->value = strtol(p, &p, 10);
            i++;
            continue;
        }

        fprintf(stderr, "Cannot tokenize: %s", p);
        exit(1);
    }

    add_token(v, TK_EOF, p);
    return v;
}
