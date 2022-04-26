#define emit(...) { printf("  "); printf(__VA_ARGS__); printf("\n"); }
#define error(...) { fprintf(stderr, __VA_ARGS__); exit(1); }

token_t skip(token_id t) {
    token_t r = tk;
    if (tk.id==t)
        next();
    else
        error("skip id=%d=%c, but got a %d\n", t, t, tk.id);
    return r;
}

bool token_eq(char *str) {
    int len = strlen(str);
    return len == tk.len && memcmp(str, tk.str, len)==0;
}

int expr();

// TERM = number | string | id | call=id(EXP) | (EXP)
int term() {
    token_t t=tk; 
    if (t.id == Id) {
        emit("load %.*s", t.len, t.str);
        next();
    } else if (t.id == Num) {
        emit("loadf %lf", tk_float); // ???
        next();
    } else if (t.id == '(') {
        skip('(');
        expr();
        skip(')');
    } else {
        error("term start with id=%d=%c, str=%10s", t.id, t.id, t.str);
    }
}

// EXPR = TERM (op TERM)*
// op = +-*/....
int expr() {
    term();
    while (tk.id >= Lor && tk.id <= Mod) {
        token_t op = tk; next();
        emit("push");
        term();
        emit("%s", tk_name(op.id));
    }
}

// STMT = (id=)?expr;
int stmt() {
    token_t id;
    bool assign = false;
    if (tk.id == Id) {
        token_t tk0 = tk;
        char *p0 = p; // 儲存掃描點
        id = skip(Id);
        if (tk.id == Assign) {
            skip(Assign);
            assign = true;
        } else {
            p = p0; // 還原掃描點到 stmt 開頭
            tk = tk0; 
        }
    }
    expr();
    if (assign) emit("store %.*s", id.len, id.str);
    skip(';');
}

// PROG = STMTS*
int prog() {
    while (tk.id != None) {
        stmt();
    }
}

int parse(char *source) {
    p = source;
    next();
    prog();
}