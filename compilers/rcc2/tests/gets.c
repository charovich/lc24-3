#include <libc/stdio.h>

U8 buf[64];

U8 main() {
    while (1) {
        puts("# ");
        gets(buf);
        puts(buf);
        puts("\n");
    }
}
