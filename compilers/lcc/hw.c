#include <stdio.h>

char buf[64];

int main() {
    char flag = 1;
    printf("Hello, World!");
    while (flag) {
        puts("# ");
        gets(buf);
        puts(buf);
        puts("\n");
    }
}
