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
