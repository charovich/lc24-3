U8 puts(U8* str) =
    "mov %dc %sp"
    "add %dc 4"
    "lh %dc %dt"
".nat:"
    "cmp *%dt $00"
    "re"
    "push *%dt"
    "int $02"
    "inx %dt"
    "jmp .nat"
;

U8 exit(U8 code) =
    "int $0"
;

U8 main() {
    puts("Loading kernel...\n\0");
    kmain();
}
