#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NMAX 100000
int src, debug, o_run, o_dump;
int line = 1;
char data[NMAX], *datap=data;
char source[NMAX], *p=source, *lp=source;
int code[NMAX], *e=code, *le=code;

// token 的 id 與 vm 的 op 共用列舉編碼
enum { // token : 0-127 直接用該字母表達， 128 以後用代號。
  None=0, Or='|',And='&',Assign='=',Xor='^',Add='+',Sub='-',Mul='*',Div='/', Mod='%', 
  AsciiEnd=128, 
  Id, Num, Str, Fn, Sys, Glo, Loc, 
  Else, If, Return, While,
  Vop0, Push, Pop,   // 虛擬機無參數運算
  Vop1, Load, Store, Vop1End, // 虛擬機單參數運算
  Op1, Inc, Dec,     // 一元運算
  Op2, Lor, Land, Eq, Neq, Le, Ge, Shl, Shr // 二元運算
};

char *op_names[] = {
  "ascii_end", 
  "id", "num", "str", "fn", "sys", "global", "loc",
  "else", "if", "return", "while",
  "vop0", "push", "pop", 
  "vop1", "load", "store", 
  "op1", "++", "--",
  "op2", "||", "&&", "==", "!=", "<=", ">=", "<<", ">>"
};

char* op_name(int op, char *name) {
  if (op < AsciiEnd)
    sprintf(name, "%c", (char) op);
  else 
    sprintf(name, "%s", op_names[op-128]);
  return name;
}

void init() {
  memset(code, 0, sizeof(code));
  memset(data, 0, sizeof(data));
}

int read_source(char *iFile) {
  FILE *fd;
  init();
  if (!(fd = fopen(iFile, "r"))) {
    printf("could not open(%s)\n", iFile);
    return -1;
  }
  int len = fread(source, 1, sizeof(source), fd);
  source[len++] = '\n'; source[len++] = '\0';
  printf("=====source=======\n%s\n==================\n", source);
  return 0;
}
