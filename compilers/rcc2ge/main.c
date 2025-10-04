#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/holy.h"
#include "lib/verconf.h"
#include "lib/reader.h"
#include "lib/tokenizer.h"
#include "lib/codegen.h"

typedef struct {
    U8* name;
    U8* code;
} NativeFunction;

U8 main(I32 argc, I8** argv) {
    U8* from = NULL;
    U8* to = NULL;
    U8 logo = 1;
    U8 help = 0;
    U8 argp = 1;
    while (argp < argc) {
        if(!strcmp(argv[argp], "--nologo")) {
            logo = 0;
            argp++;
        } else if (!strcmp(argv[argp], "--help")) {
            from = NULL;
            help = 1;
            break;
        } else {
            if (from) {
                return error("Can't compile more than one file");
            }
            from = argv[argp];
            argp++;
        }
    }
    if(logo) {
        fprintf(stderr,
                "Roman C Compiler Verdton 2.%d.%d for gc24\n"
                "Copyright (c) t.me/pyproman. All rights keked.\n",
                VER_MINOR, VER_PATCH);
        if (INDEV) {
#define INDEV_BANNER(x, ...) fprintf(stderr, "\x1b[43m\x1b[K " x "\x1b[0m\n", ## __VA_ARGS__ )
            INDEV_BANNER("\x1b[3mThis is a prototype, built on " __DATE__ ".");
            INDEV_BANNER("By GovnoCode, For GovnoCode(TM) Edition");

            if(from) INDEV_BANNER("Learn more: %s --help", argv[0]);
        }
        fprintf(stderr, "\n");
    }
    if (!from) {
        printf("usage: %s [--nologo] infile\n", argv[0]);
        printf("\n"
               "\x1b[1;4mSupported features\x1b[0m\n"
               "Functions with return type U0\n"
               "Global arrays\n"
               "Simple math: +, -, *, /\n"
               "'a = b' is actually '*a = b'\n"
               "Function calling with 1/no argument(s) (GovnPy ABI)\n"
               "  No type checking yet\n"
               "\x1b[1;4mC library\x1b[0m\n"
               "\x1b[33mU0\x1b[0m puts\x1b[33m(U8*\x1b[0m str\x1b[33m)\x1b[0m\n"
               "  Equalient to printf(\"%%s\", str)\n"
               "\x1b[33mU8\x1b[0m getchar\x1b[33m()\x1b[0m\n"
               "  Get 1 character\n"
               "\x1b[33mU0\x1b[0m beep\x1b[33m(U24\x1b[0m hz\x1b[33m)\x1b[0m\n"
               "  Beep...\n"
               "\x1b[33mU0\x1b[0m sleep\x1b[33m(U24\x1b[0m time\x1b[33m)\x1b[0m\n"
               "  Sleep\n"
               "\x1b[33mU16\x1b[0m date\x1b[33m()\x1b[0m\n"
               "  Get GovnDate\n"
               "\x1b[33mU16\x1b[0m rand\x1b[33m()\x1b[0m\n"
               "  Random number\n"
               "\x1b[33mU0\x1b[0m puti\x1b[33m(U24\x1b[0m num\x1b[33m)\x1b[0m\n"
               "  Equalient to printf(\"%%07d\", num)\n"
               "\x1b[33mU0\x1b[0m flip\x1b[33m()\x1b[0m\n"
               "  Flush videobuffer\n"
               "\x1b[33mU0\x1b[0m cls\x1b[33m()\x1b[0m\n"
               "  Clear videobuffer\n"
        );
        return help?0:1;
    }
    Reader r;
    reader_init(&r, fopen(from, "rb"));
    if(!(r.fd)) {
        fprintf(stderr, "%s not found\n", from);
        return 1;
    }
    reader_next(&r);
    Tokenizer p;
    p.rd = &r;
    tokenizer_next(&p);
    // RTL 🤑🤑🤑🤑🤑
    printf("; _start\n");
    printf("call main int 0 hlt\n");
    printf("; C library\n");
    NativeFunction nfs[] = {
        {.name = "puts", .code="pop %di pop %dt call .native push 0 push %di ret\n  .native: lb %dt %ac cmp %ac 0 re push %ac int 2 jmp .native"},
        {.name = "getchar", .code = "pop %ac int 1 push %ac ret"},
        {.name = "beep", .code = "pop %ac int $23 push 0 push %ac ret"},
        {.name = "sleep", .code = "pop %ac pop %dc int $22 push 0 push %ac ret"},
        {.name = "date", .code = "pop %ac int $3 push %dc push %ac ret"},
        {.name = "rand", .code = "pop %ac int $21 push %dc push %ac ret"},
        {.name = "puti", .code = "pop %di pop %ac mov %bs 10000000 .loop: div %ac %bs add %ac '0' push %ac int 2 mov %ac %dc div %bs 10 cmp %dc 1 jmne .loop push 0 push %di ret"},
        {.name = "flip", .code = "pop %ac int $11 push 0 push %ac ret"},
        {.name = "cls", .code = "pop %ac int $12 push 0 push %ac ret"},
        {.name = "exit", .code = "int $0"},
    };
    for (U8 i = 0; i < sizeof(nfs) / sizeof(nfs[0]); i++) {
        printf("%s: %s\n", nfs[i].name, nfs[i].code);
    }
    while(p.cur.type != EOFToken) {
        if(parse_decl(&p)!=Ok) {
            return 1;
        }
    }
    return 0;
}
