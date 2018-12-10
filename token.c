#include "shun2cc.h"

Map *keywords;

static Token *add_token(Vector *v, int type, char *asString)
{
    Token *t = malloc(sizeof(Token));
    t->type = type;
    t->asString = asString;
    vec_push(v, t);
    return t;
}

static Vector *scan(char *p)
{
    Vector *v = new_vec();

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (strchr("+-*/;=()", *p)) {
            add_token(v, *p, p);
            p++;
            continue;
        }

        if (isalpha(*p) || *p == '_') {
            int len = 1;
            while (isalpha(p[len]) || isdigit(p[len]) || p[len] == '_') {
                len++;
            }

            char *name = strndup(p, len);
            int type = (intptr_t)map_get(keywords, name);
            if (!type) {
                type = TK_IDENT;
            }

            Token *t = add_token(v, type, p);
            t->name = name;
            p += len;
            continue;
        }

        if (isdigit(*p)) {
            Token *t = add_token(v, TK_NUM, p);
            t->value = strtol(p, &p, 10);
            continue;
        }

        error("Cannot tokenize: %s", p);
    }

    add_token(v, TK_EOF, p);
    return v;
}


Vector *tokenize(char *p)
{
    keywords = new_map();
    map_put(keywords, "return", (void *)TK_RETURN);

    return scan(p);
}