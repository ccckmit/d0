#include <lex.c>
// #define emit(...) { printf("  "); printf(__VA_ARGS__); printf("\n"); }
#define error(...) { fprintf(stderr, __VA_ARGS__); exit(1); }

token_t skip(int t) {
    token_t r = tk;
    if (tk.id==t)
        next();
    else
        error("skip id=%d=%c, but got a %d\n", t, t, tk.id);
    return r;
}

bool match(char *str) {
    int len = strlen(str);
    return len == tk.len && memcmp(str, tk.str, len)==0;
}

token_t tk0; char *p0;
void scan_save() { tk0 = tk; p0=p; }
void scan_restore() { tk = tk0; p=p0; }

void emit1(int op) { *e++=op; }
void emit2(int op, sym_t *sym) { *e++=op; *e++=sym-symtb; }

int expr();

// TERM = number | string | id | (EXP) | [-~!] TERM ([EXP] | (EXP)) 
int term() {
    token_t t=tk; 
    if (t.id == Id || t.id == Num || t.id == Str) {
        emit2(Load, tk.sym);
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
    while (strchr("|&=^+-*/%", tk.id) || tk.id >= Op2) {
        token_t op = tk; next();
        emit1(Push);
        term();
        emit1(op.id);
    }
}

// STMT = (id=)?expr;
int stmt() {
    token_t id;
    bool assign = false;
    if (tk.id == Id) {
        scan_save();
        id = skip(Id);
        if (tk.id == '=') {
            skip('=');
            assign = true;
        } else {
            scan_restore(); // 還原掃描點到 stmt 開頭
        }
    }
    expr();
    if (assign) emit2(Store, id.sym);
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