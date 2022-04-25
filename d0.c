#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NMAX 100000
int src, debug, o_run, o_dump;
int line;
char data[NMAX], *datap=data;
char source[NMAX], *p=source, *lp=source;
int code[NMAX], *e=code, *le=code;

// Token
enum token { // token : 0-127 直接用該字母表達， 128 以後用代號。
  None, Num = 128, Str, Fun, Sys, Glo, Loc, Id,
  Char, Else, Enum, If, Int, Return, Sizeof, While,
  Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak
};
enum token tk;   // current token (目前 token)
double tk_float; // token 的浮點值
char *tk_str;    // token 的字串
int tk_len;      // token 的字串長度

char st[NMAX], *stp = st;

char *st_add(char *str, int len) {
    char *s = stp;
    memcpy(stp, str, len);
    stp += len;
    *stp++ = '\0';
    return s;
}

struct symbol {
    char *name;
    int  token, type, value;
    // Tk, Hash, Name, Class, Type, Val, HClass, HType, HVal, Idsz
};

#define NSYM 9997
struct symbol sym[NSYM];

int hash(char *name, int len) { // 計算雜湊值
    char *p = name;
    int h=1;
    while ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') || *p == '_')
        h = h * 147 + *p++;
    h = (h << 6) + len;
    return h;
}

int sym_init() {
    memset(sym, 0, sizeof(struct symbol)*NSYM);
}

struct symbol *sym_find(char *name, int len, bool *found) {
    int si = hash(name, len)%NSYM, i=si;
    while (1) {
        if (!sym[i].name) { *found=false; return &sym[i]; }
        if (memcmp(sym[i].name, name, len)==0) { *found=true; return &sym[i]; }
        if (++i == si) { *found=false; return NULL; }
    }
}

struct symbol *sym_add(char *name, int len) {
    bool found;
    struct symbol *s = sym_find(name, len, &found);
    if (!found) s->name = st_add(name, len);
    return s;
}

int scan_str(char *str, char *nstr) {
    char *p = str, ch, *np=nstr;
    if (*p != '\'') return 0;
    p++;
    while (*p != 0 && *p != '\'') {
        if ((ch = *p++) == '\\') {
            if ((ch = *p++) == 'n') ch = '\n'; // 處理 \n 的特殊情況
        }
        *np++ = ch; // 把字串塞到 nstr 裏
    }
    return p - str;
}

void next() { // 詞彙解析 lexer
  char ch;
  tk = None;
  while (ch = *p) {
    tk_str = p++;
    if (ch == '\n') { // 換行
      if (src) {
        printf("%d: %.*s", line, p - lp, lp); // 印出該行
        /*
        lp = p; // lp = p = 新一行的原始碼開頭
        while (le < e) { // 印出上一行的所有目的碼
          printInstr(le, code, data);
          le = le + stepInstr(le);
        }
        */
      }
      ++line;
    }
    // else if (ch == '#') { // 取得 #include <stdio.h> 這類的一整行
    //   while (*p != 0 && *p != '\n') ++p;
    // }
    else if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_') { // 取得變數名稱
      while ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') || *p == '_')
          *p++;
      tk = Id;
      break;
    }
    else if (ch >= '0' && ch <= '9') { // 取得數字串
      while ((*p>='0' && *p<='9') || (*p=='.')) p++;
      sscanf(tk_str, "%f", &tk_float);
      tk = Num;
      break;
    }
    else if (ch == '/') {
      if (*p == '/') { // 註解
        ++p;
        while (*p != 0 && *p != '\n') ++p; // 略過註解
      }
      else { // 除法
        tk = Div;
        break;
      }
    }
    else if (ch == '\'') { // 字元或字串
      int len = scan_str(tk_str, datap);
      // printf("tk_str=%.*s", len, tk_str);
      p += len;
      tk = Str;
      break;
    } // 以下為運算元 =+-!<>|&^%*[?~, ++, --, !=, <=, >=, ||, &&, ~  ;{}()],:
    else if (ch == '=') { if (*p == '=') { ++p; tk = Eq; } else tk = Assign; break; }
    else if (ch == '+') { if (*p == '+') { ++p; tk = Inc; } else tk = Add; break; }
    else if (ch == '-') { if (*p == '-') { ++p; tk = Dec; } else tk = Sub; break; }
    else if (ch == '!') { if (*p == '=') { ++p; tk = Ne; } break; }
    else if (ch == '<') { if (*p == '=') { ++p; tk = Le; } else if (*p == '<') { ++p; tk = Shl; } else tk = Lt; break; }
    else if (ch == '>') { if (*p == '=') { ++p; tk = Ge; } else if (*p == '>') { ++p; tk = Shr; } else tk = Gt; break; }
    else if (ch == '|') { if (*p == '|') { ++p; tk = Lor; } else tk = Or; break; }
    else if (ch == '&') { if (*p == '&') { ++p; tk = Lan; } else tk = And; break; }
    else if (ch == '^') { tk = Xor; break; }
    else if (ch == '%') { tk = Mod; break; }
    else if (ch == '*') { tk = Mul; break; }
    else if (ch == '[') { tk = Brak; break; }
    else if (ch == '?') { tk = Cond; break; }
    else { tk=ch; break; } // if (strchr("~;{}()],:", ch)) 
  }
  tk_len = p-tk_str;
}

int lex(char *source) {
    p = source;
    while (true) {
        next();
        if (tk == None) break;
        printf("%2d:%d %.*s\n", tk, tk_len, tk_len, tk_str);
    }
}

void init() {
  memset(sym,  0, sizeof(sym));
  memset(code, 0, sizeof(code));
  memset(data, 0, sizeof(data));
}

int main(int argc, char **argv) {
  char *iFile, *oFile, *narg;
  FILE *fd;
  int o_save;
  // 主程式
  --argc; ++argv; // 略過執行檔名稱
  if (argc > 0 && **argv == '-' && (*argv)[1] == 's') { src = 1; --argc; ++argv; }
  if (argc > 0 && **argv == '-' && (*argv)[1] == 'd') { debug = 1; --argc; ++argv; }
  if (argc > 0 && **argv == '-' && (*argv)[1] == 'r') { o_run = 1; --argc; ++argv; }
  if (argc > 0 && **argv == '-' && (*argv)[1] == 'u') { o_dump = 1; --argc; ++argv; }
  if (argc < 1) { printf("usage: d0 [-s] [-d] [-r] [-u] in_file [-o] out_file...\n"); return -1; }
  iFile = *argv;
  if (argc > 1) {
    narg = *(argv+1);
    if (*narg == '-' && narg[1] == 'o') {
      o_save = 1;
      oFile = *(argv+2);
    }
  }
  if (!(fd = fopen(iFile, "r"))) {
    printf("could not open(%s)\n", iFile);
    return -1;
  }
  init();
  fread(source, 1, sizeof(source), fd);
  lex(source);
  // if (compile(fd)==-1) return -1; // 編譯
}

 