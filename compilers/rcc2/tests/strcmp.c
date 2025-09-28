#include <libc/stdio.h>

U8 buf[64];

U8 main() {
    while (1) {
        puts("# ");
        gets(buf);
        if (strcmp(buf, "bye")) {
            puts("Bye!\n");
        }
    }
}
