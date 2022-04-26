#include <obj.c>

char st[NMAX], *stp = st;

char *st_add(char *str, int len) {
    char *s = stp;
    memcpy(stp, str, len);
    stp += len;
    *stp++ = '\0';
    return s;
}

typedef struct symbol {
    char *name;
    Obj *obj;
} symbol_t;

#define NSYM 9997
symbol_t sym[NSYM];

int hash(char *name, int len) { // 計算雜湊值
    char *p = name;
    int h=1;
    while ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') || *p == '_')
        h = h * 147 + *p++;
    h = (h << 6) + len;
    return h;
}

int sym_init() {
    memset(sym, 0, sizeof(sym));
}

symbol_t *sym_find(char *name, int len, bool *found) {
    int si = hash(name, len)%NSYM, i=si;
    while (1) {
        if (!sym[i].name) { *found=false; return &sym[i]; }
        if (memcmp(sym[i].name, name, len)==0) { *found=true; return &sym[i]; }
        if (++i == si) { *found=false; return NULL; }
    }
}

symbol_t *sym_get(char *name, int len) {
    bool found;
    symbol_t *s = sym_find(name, len, &found);
    return s;
}

symbol_t *sym_add(char *name, int len) {
    bool found;
    symbol_t *s = sym_find( name, len, &found);
    if (!found) s->name = st_add(name, len);
    return s;
}
