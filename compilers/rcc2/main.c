///$(which gcc) $0 -o ball && exec ./ball "$@" || exit $?
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/holy.h"
#include "lib/verconf.h"

#include "lib/reader.h"
#include "lib/preproc.h"
#include "lib/tokenizer2.h"

#include "lib/decl.h"
#include "lib/env.h"
#include "lib/declpars.h"

#include "lib/typechk.h"
#include "lib/parser2.h"
#include "lib/codegen2.h"

#include "lib/targets/lostasm.h"
#include "lib/targets/lc24.h"

U8 main(I32 argc, U8** argv) {
    U32 i = 0;
    U8* from = 0;
    U8* to = 0;
    U8 export = 0;
    Target* tgt = 0;

    for (U32 i = 1; i < argc; i++) {
        if(!strcmp(argv[i], "--help")) {
            printf("rcc2 source.c [-o target.s]\n");
            printf("Compiles (Holy)C programs to Lost AS\n");
            return 0;
        } else if (!strcmp(argv[i], "-o")) {
            if (i + 1 == argc) {
                fprintf(stderr, "rcc2: nothing provided for -o\n");
                return 1;
            }
            if (to) {
                fprintf(stderr, "rcc2: Can't output to multiple files\n");
                return 1;
            }
            to = argv[i + 1];
            i++;
        } else if (!strcmp(argv[i], "-t")) {
            if (i + 1 == argc) {
                fprintf(stderr, "rcc2: nothing provided for -t\n");
                return 1;
            }
            if (tgt) {
                fprintf(stderr, "rcc2: Multiple -t\n");
                return 1;
            }
#define target(name) if (!strcmp(argv[i + 1], #name)) tgt = &name; else
            target(lc24)
            /*else*/{
                fprintf(stderr, "rcc2: Unknown target %s\n", argv[i + 1]);
                return 1;
            }
#undef target
        } else {
            if (from) {
                fprintf(stderr, "rcc2: Only one input file is supported\n");
                return 1;
            }
            from = argv[i];
        }
    }
    if (!tgt) tgt = &lc24;
    if (!from) {
        fprintf(stderr, "rcc2: No input file provided\n");
        return 1;
    }
    if (!to) {
        // try like this: main.c -> main.s
        U32 fl = strlen(from);
        if (fl > 2 && !strcmp(from + fl - 2, ".c")) {
            to = strdup(from);
            to[fl - 1] = 's';
        } else {
            to = "a.out";
        }
    }

    FILE* src = fopen(from, "r");
    if (!src) {
        fprintf(stderr, "rcc2: file \"%s\" not found\n", from);
        return 1;
    }

    FILE* dst = fopen(to, "w");
    if (!dst) {
        fprintf(stderr, "rcc2: can't open \"%s\"\n", to);
        return 1;
    }

    Reader r;
    r.fd_i = 0;
    r.fd[0].fd = 0;
    reader_include(&r, src, from);
    reader_next(&r);

    Tokenizer p;
    p.rd = &r;
    tokenizer_next(&p);

    if (holyc_parody(dst, &lc24, &p) != Ok) return 1;

    return 0;
}
