#include <init.c>
#include <lex.c>
#include <parser.c>

int main(int argc, char **argv) {
    read_source(argv[1]);
    src = true;
    parse(source);
}
