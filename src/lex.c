#include <sym.c>

// Token
typedef enum token_id { // token : 0-127 直接用該字母表達， 128 以後用代號。
  Id=128, Num, Str, Fun, Sys, Glo, Loc, 
  Else, If, Return, While, Assign, Inc, Dec,
  Lor, Land, Or, Xor, And, Eq, Neq, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod,
  None,
} token_id;

char *token_name[] = {
  "id", "num", "str", "fun", "sys", "glo", "loc", 
  "else", "if", "return", "while", "assign", "inc", "dec",
  "lor", "land", "or", "xor", "and", "eq", "neq", "lt", "gt", "le", "ge", "shl", "shr", "add", "sub", "mul", "div", "mod",
  "none"
};

char *tk_name(token_id id) {
  if (id < 128) return "char";
  if (id >= None) return "none";
  return token_name[id-128];
}

typedef struct token_t {
    token_id id;
    char *str; // token 的字串
    int len;   // token 的字串長度
} token_t;

token_t tk;   // current token (目前 token)
double tk_float; // token 的浮點值

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
  tk.id = None;
  while (*p) {
    tk.str = p;
    char ch = *p++;
    if (ch == '\n') { // 換行
      if (src) {
        printf("%d: %.*s", line, p - lp, lp); // 印出該行
        lp = p; // lp = p = 新一行的原始碼開頭
        /*
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
      tk.id = Id;
      break;
    }
    else if (ch >= '0' && ch <= '9') { // 取得數字串
      while ((*p>='0' && *p<='9') || (*p=='.')) p++;
      sscanf(tk.str, "%lf", &tk_float);
      tk.id = Num;
      break;
    }
    else if (ch == '/') {
      if (*p == '/') { // 註解
        ++p;
        while (*p != 0 && *p != '\n') ++p; // 略過註解
      }
      else { // 除法
        tk.id = Div;
        break;
      }
    }
    else if (ch == '\'') { // 字元或字串
      int len = scan_str(tk.str, datap);
      // printf("tk.str=%.*s", len, tk.str);
      p += len;
      tk.id = Str;
      break;
    } // 以下為運算元 =+-!<>|&^%*[?~, ++, --, !=, <=, >=, ||, &&, ~  ;{}()],:
    else if (ch == '=') { if (*p == '=') { ++p; tk.id = Eq; } else tk.id = Assign; break; }
    else if (ch == '+') { if (*p == '+') { ++p; tk.id = Inc; } else tk.id = Add; break; }
    else if (ch == '-') { if (*p == '-') { ++p; tk.id = Dec; } else tk.id = Sub; break; }
    else if (ch == '!') { if (*p == '=') { ++p; tk.id = Neq; } break; }
    else if (ch == '<') { if (*p == '=') { ++p; tk.id = Le; } else if (*p == '<') { ++p; tk.id = Shl; } else tk.id = Lt; break; }
    else if (ch == '>') { if (*p == '=') { ++p; tk.id = Ge; } else if (*p == '>') { ++p; tk.id = Shr; } else tk.id = Gt; break; }
    else if (ch == '|') { if (*p == '|') { ++p; tk.id = Lor; } else tk.id = Or; break; }
    else if (ch == '&') { if (*p == '&') { ++p; tk.id = Land; } else tk.id = And; break; }
    else if (ch == '^') { tk.id = Xor; break; }
    else if (ch == '%') { tk.id = Mod; break; }
    else if (ch == '*') { tk.id = Mul; break; }
    else if (strchr("~;{}()[],:", ch)) { tk.id=ch; break; }
    else if (strchr(" \n\r\t", ch)) {} // 空格，繼續往前讀...
  }
  tk.len = p-tk.str;
}

int lex(char *source) {
    p = source;
    while (true) {
        next();
        if (tk.id == None) break;
        printf("%2d:%d %.*s\n", tk, tk.len, tk.len, tk.str);
    }
}