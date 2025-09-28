#include "holy.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

U8* chars[] = {
// Sorted by (infix) operator order
    "++", "PlusPlus",
    "--", "MinusMinus",
    "(", "LeftParen",
    ")", "RightParen",
    "[", "LeftBracket",
    "]", "RightBracket",
    ".", "Dot",
    "->", "Arrow",

    "!", "Not",
    "~", "Tilde",
    // Prefix ++, --, * and & are also here

    "*", "Star",
    "/", "Div",
    "%", "Mod",

    "+", "Plus",
    "-", "Minus",

    "<<", "LeftShift",
    ">>", "RightShift",

    "<", "Less",
    "<=", "LessEqual",
    ">", "Greater",
    ">=", "GreaterEqual",

    "==", "Equal",
    "!=", "NotEqual",

    "&", "And",
    "^", "Caret",
    "|", "Or",

    "&&", "AndAnd",

    "||", "OrOr",

    "?", "Question",
    ":", "Colon",


    "=", "Assign",
    "*=", "StarAssign",
    "/=", "DivAssign",
    "%=", "ModAssign",
    "+=", "PlusAssign",
    "-=", "MinusAssign",
    "<<=", "LeftShiftAssign",
    ">>=", "RightShiftAssign",
    "&=", "AndAssign",
    "^=", "XorAssign",
    "|=", "OrAssign",

    ",", "Comma",

    ";", "Semi",

    "..", "DoubleDot",
    "...", "TripleDot",

    "{", "LeftBrace",
    "}", "RightBrace",
    "0", (
        "!"
        "reader_next(tok->rd);"
        "switch (tok->rd->cur) {"
        "    case 'x': {\n"
        "        reader_next(tok->rd);\n"
        "        if (!isxdigit(tok->rd->cur)) {\n"
        "            return tok_fail(tok, \"Zero-length hex\");"
        "        }\n"
        "        U32 c = 0;"
        "        while (isxdigit(tok->rd->cur)) {\n"
        "            c = c * 16 + decx(tok->rd->cur);\n"
        "            reader_next(tok->rd);\n"
        "        }\n"
        "        tok->cur.type = NumberToken;\n"
        "        tok->cur.number = c;\n"
        "        break;"
        "    }\n"
        "    default:\n"
        "        tok->cur.type = NumberToken;\n"
        "        tok->cur.number = 0;\n"
        "}"
    ),
    "'", (
        "!"
        "U8 c;\n"
        "if (!read_escape(tok->rd, &c, '\\'')) {\n"
        "    return tok_fail(tok, \"0 chars provided\");\n"
        "}\n"
        "reader_next(tok->rd);\n"
        "if (tok->rd->cur != '\\'') {\n"
        "    return tok_fail(tok, \"Too many chars provided\");\n"
        "}\n"
        "reader_next(tok->rd);\n"
        "tok->cur.type = NumberToken;\n"
        "tok->cur.number = c;"
    ),
    "#", (
        "!"
        "reader_next(tok->rd);\n"
        "return preprocessor(tok->rd) == Ok ? tokenizer_next(tok) : tok_fail(tok, \"Preprocessor error\");"
    ),
    "//", (
        "!"
        "reader_next(tok->rd);\n"
        "while(tok->rd->cur && tok->rd->cur != '\\n')"
            "reader_next(tok->rd);\n"
        "return tokenizer_next(tok);"
    ),
    "/*", "!return;",
    0
};
U8 null;

typedef void* (*TTable);
U0 add(TTable table, U8* str, U8* map) {
    U8 c = str[0];
    if (!c) {
        table[c] = map;
        return;
    }
    if (!table[c]) {
        table[c] = calloc(256, sizeof(U8*));
    }

    add(table[c], str + 1, map);
}
U0 print(TTable table, U8 depth) {
    U8 flag = 0;
    for (U8 i = 1; i < 127; i++) {
        if (table[i]) {
            if (!flag) {
                printf("%*s%scur = reader_next(tok->rd);\n", depth * 4, "", depth > 1 ? "" : "U8 ");
                flag = 1;
            }
            printf("%*sif (cur == '%c') {\n", depth * 4, "", i);
            print(table[i], depth+1);
            printf("%*s}\n", depth * 4, "");
        }
    }
    if(table[0]) {
        U8* data = (U8*)table[0];
        if(data[0] == '!') {
            printf("%*s", depth * 4, "");
            for(U16 i = 1; data[i]; i++) {
                if (data[i] == '\n') {
                    printf("\n%*s", depth * 4, "");
                } else {
                    printf("%c", data[i]);
                }
            }
            printf("\n");
        } else {
            if (!flag) {
                printf("%*sreader_next(tok->rd);\n", depth * 4, "");
            }
            printf("%*stok->cur.type = %s;\n", depth * 4, "", data);
            if (depth > 1) {
                printf("%*sreturn;\n", depth * 4, "");
            }
        }
    }
}
U8 main() {
    TTable table = calloc(256, sizeof(U8*));
    printf(
        "typedef enum {\n"
        "    EOFToken, // 00\n"
        // "    KeywordToken,\n"
        "    NameToken, // 01\n"
        "    NumberToken, // 02\n"
        "    StrToken, // 03\n"
    );
    for (U16 i = 0; chars[i]; i += 2) {
        add(table, chars[i], chars[i+1]);
        if(chars[i+1][0] != '!') printf("    %s, // %02X\n", chars[i+1], 4 + i / 2);
    }
    printf(
        "    IfKeywordToken,\n"
        "    ElseKeywordToken,\n"
        "    WhileKeywordToken,\n"
        "    ForKeywordToken,\n"
        "    TypedefKeywordToken,\n"
        "    StructKeywordToken,\n"
        "    UnionKeywordToken,\n"
        "    EnumKeywordToken,\n"
        "    ReturnKeywordToken\n"
        "} TokenType;\n"
        "typedef struct {\n"
        "    TokenType type;\n"
        "    union {\n"
        "        U32 number;\n"
        "        I8 str[NAME_LIMIT+1];\n"
        "        U8* ptr;\n"
        "        ERR err;\n"
        "    };\n"
        " } Token;\n"
        "\n"
        "typedef struct {\n"
        "    Reader* rd;\n"
        "    Token cur;\n"
        "} Tokenizer;\n"
        "\n"
        "#define getk(tkptr, tktype) if (tkptr->cur.type != tktype) { tkerr(tkptr, \"Expected \" #tktype); return Error; }\n"
        "#define decx(c) (c >= 'A' ? 9 + (c & 159) : c - '0')\n"
        "U0 tokenizer_next(Tokenizer* tok);\n"

        "U0 tok_fail(Tokenizer* tok, I8* err) {\n"
        "    tkerr(tok, \"%%s\", err);\n"
        "    tok->cur.type = EOFToken;\n"
        "    tok->cur.err = Error;\n"
        "}\n"
        "U0 tok_unk(Tokenizer* tok) {\n"
        "    return tok_fail(tok, \"Unknown character\");\n"
        "}\n"
        "U8 read_escape(Reader* rd, U8* out, U8 stop) {\n"
        "    reader_next(rd);\n"
        "    if (!rd->cur) {\n"
        "        *out = 0;\n"
        "        return 0;\n"
        "    } else if (rd->cur == '\\\\') {\n"
        "        reader_next(rd);\n"
        "        if (rd->cur == 'x') {\n"
        "            reader_next(rd);\n"
        "            if (!isxdigit(rd->cur)) {\n"
        "                *out = 0;\n"
        "                return 0;\n"
        "            }\n"
        "            U8 c = decx(rd->cur) * 16;\n"
        "            reader_next(rd);\n"
        "            if (!isxdigit(rd->cur)) {\n"
        "                *out = 0;\n"
        "                return 0;\n"
        "            }\n"
        "            c += decx(rd->cur);\n"
        "            *out = c;\n"
        "        } else if (rd->cur == 'n') {\n"
        "            *out = '\\n';\n"
        "        } else if (rd->cur == 'b') {\n"
        "            *out = '\\b';\n"
        "        } else {\n"
        "            *out = rd->cur;\n"
        "        }\n"
        "        return 1;\n"
        "    } else if (rd->cur == stop) {\n"
        "        *out = 0;\n"
        "        return 0;\n"
        "    }\n"
        "    *out = rd->cur;\n"
        "    return 1;\n"
        "}\n"
        "U8* keywords[] = {\"if\", \"else\", \"while\", \"for\", \"typedef\", \"struct\", \"union\", \"enum\", \"return\", 0};\n"
        "U0 tok_var(Tokenizer* tok) {\n"
        "    U8 c = tok->rd->cur, i = 0;\n"
        "    while (c == '_' || isalnum(c)) {\n"
        "        if (i == NAME_LIMIT) return tok_fail(tok, \"Variable limit reached\");"
        "        tok->cur.str[i++] = c;\n"
        "        c = reader_next(tok->rd);\n"
        "    }\n"
        "    tok->cur.str[i] = 0;\n"
        "    for(i = 0; keywords[i]; i++) {\n"
        "        if(!strcmp(keywords[i], tok->cur.str)) {\n"
        "            tok->cur.type = IfKeywordToken + i;\n"
        "            return;"
        "        }\n"
        "    }\n"
        "    tok->cur.type = NameToken;\n"
        "}\n"
        "U0 tok_str(Tokenizer* tok) {\n"
        "    U8* m = malloc(256);\n"
        "    U8 i = 0;\n"
        "    while (read_escape(tok->rd, &(m[i++]), '\"')) ;\n"
        "    if (tok->rd->cur != '\"') return tok_fail(tok, \"String not finished\");\n"
        "    reader_next(tok->rd);\n"
        "    tok->cur.type = StrToken;\n"
        "    tok->cur.ptr = m;\n"
        "}\n"
        "U0 tok_num(Tokenizer* tok) {\n"
        "    U32 i = 0;\n"
        "    while (isdigit(tok->rd->cur)) {\n"
        "        i = i * 10 + (tok->rd->cur - '0');\n"
        "        reader_next(tok->rd);\n"
        "    }\n"
        "    tok->cur.type = NumberToken;\n"
        "    tok->cur.number = i;\n"
        ""
        "}\n"
        "U0 tok_eof(Tokenizer* tok) {\n"
        "    tok->cur.type = EOFToken;\n"
        "    tok->cur.err = Ok;\n"
        "}\n");
    for (U8 i = 0; i < 127; i++) {
        if (table[i]) {
            printf("U0 tok_on_%02x(Tokenizer* tok) { // '%c'\n", i, i);
            print(table[i], 1);
            printf("}\n");
        }
    }
    printf(
        "typedef U0 (*TokNxFn)(Tokenizer* tok);\n"
        "TokNxFn nxfns[128] = {\n");

    for (U8 i = 0; i < 128; i++) {
        if (table[i])
            printf("&tok_on_%02x, ", i);
        else if (isdigit(i))
            printf("&tok_num,   ");
        else if (i == '_' || isalpha(i))
            printf("&tok_var,   ");
        else if (i == '"')
            printf("&tok_str,   ");
        else if (i)
            printf("&tok_unk,   ");
        else
            printf("&tok_eof,   ");
        if (i % 16 == 15) printf("\n");
    }
    printf(
        "};\n"
        "U0 tokenizer_next(Tokenizer* tok) {\n"
        "    U8 c = tok->rd->cur;\n"
        "    while (isspace(c)) c = reader_next(tok->rd);\n"
        "    return nxfns[c>127?'_':c](tok);\n"
        "}\n");
    printf(
        "U8* token_name(Token* tok) {\n"
        "    switch (tok->type) {\n"
        "        case EOFToken: return \"EOF\";\n"
        "        case IfKeywordToken: return \"if\";\n"
        "        case WhileKeywordToken: return \"while\";\n"
        "        case ForKeywordToken: return \"for\";\n"
        "        case NameToken: return \"<name>\";\n"
        "        case NumberToken: return \"<int>\";\n"
        "        case StrToken: return \"<str>\";\n");
    for (U16 i = 0; chars[i]; i += 2) {
        if(chars[i+1][0] != '!') printf("        case %s: return \"%s\\\"%s\\\"\"; \n", chars[i+1], chars[i+1], chars[i]);
    }
    printf(
        "    }\n"
        "    return \"???\";\n"
        "}\n");
    return 0;
}
