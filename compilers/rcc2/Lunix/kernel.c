#include <libs/stdio.h>

U8 kmain() {}
U8 main() {}

U8 boot() {
    puts("Loading...\n");
}

U8 main() {
    boot();
    puts("In kernel lol\n");
    kmain();
    retrn(0);
}
