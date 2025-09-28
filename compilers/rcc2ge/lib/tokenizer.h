// Roman C Compiler 2 tokenizer
// Copyright (C) 2025 t.me/pyproman
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.


typedef enum {
    EOFToken,
    KeywordToken,
    NameToken,
    NumberToken,
    StrToken
} TokenType;
typedef struct {
    TokenType type;
    union {
        U32 number;
        U8 str[16];
        U8* ptr;
        ERR err;
    } data;
} Token;

typedef struct {
    Reader* rd;
    Token cur;
} Tokenizer;

#define tkerr(tkptr, msg, ...) (fprintf(stderr, "on %d:%d:\n\x1b[31m" msg "\x1b[0m\n", tkptr->rd->line, tkptr->rd->col, ## __VA_ARGS__), 1)

#define IGNORE(x) 0
U8 read_escape(Reader* rd, U8* out, U8 stop) {
    IGNORE("Hello world\" \\ \n \x1b[1m cbo");
    reader_next(rd);
    if(rd->cur == '\\') {
        reader_next(rd);
        if (rd->cur == 'x') {
            // Todo
            reader_next(rd);
            reader_next(rd);
            *out = 0x1b;
        } else if (rd->cur == 'n') {
            *out = '\n';
        } else if (rd->cur == 'b') {
            *out = '\b';
        } else {
            *out = rd->cur;
        }
        return 1;
    } else if (rd->cur == stop) {
        *out = 0;
        return 0;
    }
    *out = rd->cur;
    return 1;
}
#define IS_NUM(x) (('0' <= x) && (x <= '9'))
#define IS_ALP(x) ((('A' <= x) && (x <= 'Z')) || (('a' <= x) && (x <= 'z')))
#define IS_BLNK(x) ((x == 0x20) || (x == 0x0a) || (x == 0x09))
#define IS_HEX(x) (IS_NUM(x) || (('A' <= x) && (x <= 'F')) || (('a' <= x) && (x <= 'f')))
#define FRM_HEX(x) (IS_NUM(x) ? ( x - '0' ) : ( (x & 0xDF) - '7')) // computed by hand(TM)
Token* tokenizer_next(Tokenizer* psr) {
    U8 cur = psr->rd->cur;
    while (IS_BLNK(cur)) {
        //if (cur == 0xa) printf("\n");
        cur = reader_next(psr->rd);
    }
    if (cur == 0) {
        psr->cur.type = EOFToken;
        psr->cur.data.err = Ok;
        return &(psr->cur);
#define C(x) (cur == x)
    } else if (C('0')) {
        psr->cur.type = NumberToken;
        U32 num = 0;
        cur = reader_next(psr->rd);
        if (IS_NUM(cur)) {
            tkerr(psr, "Octal numbers are not supported");
            psr->cur.type = EOFToken;
            psr->cur.data.err = Error;
            return &(psr->cur);
        } else if (cur == 'x') {
            while(cur = reader_next(psr->rd), IS_HEX(cur))
                num = num * 16 + FRM_HEX(cur);
        }
        psr->cur.data.number = num;
        return &(psr->cur);
    } else if (IS_NUM(cur)) {
        psr->cur.type = NumberToken;
        U32 num = cur - '0';
        while(cur = reader_next(psr->rd), IS_NUM(cur))
            num = num * 10 + (cur - '0');

        psr->cur.data.number = num;
        return &(psr->cur);
    } else if (IS_ALP(cur)) {
        psr->cur.type = NameToken;
        psr->cur.data.str[0] = cur;
        U8 i = 1;
        while (cur = reader_next(psr->rd), IS_ALP(cur) || IS_NUM(cur) || (cur == '_')) {
            psr->cur.data.str[i] = cur;
            if ((++i) >= 15) {
                tkerr(psr, "Urn't shkspr bro, I aint readng al of tht");
                psr->cur.type = EOFToken;
                psr->cur.data.err = Error;
                return &(psr->cur);
            }
        }
        psr->cur.data.str[i] = 0;
        // Compare
#define S(x) (!strcmp(psr->cur.data.str, x))
        if (S("U0") || S("U8") || S("typedef") || S("struct") || S("return") || S("if") || S("while") || S("enum") || S("for") || S("else") || S("break") || S("continue")) {
            psr->cur.type = KeywordToken;
        }
#undef S
        return &(psr->cur);
    } else if (C('/')) {
        cur = reader_next(psr->rd);
        if (C('/')) {
            while (!(C(0x0a) || C(0x00))) cur = reader_next(psr->rd);
            return tokenizer_next(psr);
        } else if (C('*')) {
            U8 star = 0;
            for (;;) {
                cur = reader_next(psr->rd);
                if((cur == '/') && star) {
                    break;
                }
                star = cur == '*';
            }
            reader_next(psr->rd);
            return tokenizer_next(psr);
        } else {
            psr->cur.type = KeywordToken;
            psr->cur.data.str[0] = '/';
            psr->cur.data.str[1] = 0;
            return &(psr->cur);
        }
    } else if (C('=') || C('<') || C('>')) {
        psr->cur.type = KeywordToken;
        psr->cur.data.str[0] = cur;
        cur = reader_next(psr->rd);
        if(C('=')) {
            psr->cur.data.str[1] = cur;
            psr->cur.data.str[2] = 0;
            reader_next(psr->rd);
        } else {
            psr->cur.data.str[1] = 0;
        }
        return &(psr->cur);
    } else if (C('-')) {
        psr->cur.type = KeywordToken;
        psr->cur.data.str[0] = cur;
        cur = reader_next(psr->rd);
        if(C('>') || C('-')) {
            psr->cur.data.str[1] = cur;
            psr->cur.data.str[2] = 0;
            reader_next(psr->rd);
        } else {
            psr->cur.data.str[1] = 0;
        }
        return &(psr->cur);
    } else if ( C('|') || C('+') || C('&')) {
        psr->cur.type = KeywordToken;
        psr->cur.data.str[0] = cur;
        if(reader_next(psr->rd) == cur) {
            psr->cur.data.str[1] = cur;
            psr->cur.data.str[2] = 0;
            reader_next(psr->rd);
        } else {
            psr->cur.data.str[1] = 0;
        }
        return &(psr->cur);
    } else if (C('[') || C(']') || C('{') || C('}') || C('(') || C(')') || C('*') || C(';') || C(',') || C('.') || C('!') || C(':')) {
        psr->cur.type = KeywordToken;
        psr->cur.data.str[0] = cur;
        psr->cur.data.str[1] = 0;
        reader_next(psr->rd);
        return &(psr->cur);
    } else if (C('"')) {
        psr->cur.type = StrToken;
        psr->cur.data.ptr = malloc(256);
        U16 i = 0;
        while(read_escape(psr->rd, &(psr->cur.data.ptr[i++]), '"'))
            ;
        psr->cur.data.ptr[i++] = 0;
        reader_next(psr->rd);
        return &(psr->cur);
    } else if (C('\'')) {
        psr->cur.type = NumberToken;
        U8 dat;
        if((!read_escape(psr->rd, &dat, '\'')) || (reader_next(psr->rd) != '\'')) {
            tkerr(psr, "Invalid escape");
            psr->cur.type = EOFToken;
            psr->cur.data.err = Error;
            return &(psr->cur);
        }
        reader_next(psr->rd);
        psr->cur.data.number = dat;
        return &(psr->cur);
    } else if (C('#')) {
        while (!(C(0x0a) || C(0x00))) cur = reader_next(psr->rd);
        return tokenizer_next(psr);
    } else {
        tkerr(psr, "Unknown character %c", cur);
        psr->cur.type = EOFToken;
        psr->cur.data.err = Error;
        return &(psr->cur);
    }
}

U0 print_token(Token* tok) {
    switch(tok->type) {
        case EOFToken:
            printf("(eof) ");
            break;
        case NumberToken:
            printf("\x1b[34m%d\x1b[0m ", tok->data.number);
            break;
        case NameToken:
            printf("%s ", tok->data.str);
            break;
        case KeywordToken:
            printf("\x1b[33m%s\x1b[0m ", tok->data.str);
            break;
        case StrToken:
            printf("\x1b[100m");
            for (U16 i = 0; tok->data.ptr[i]; i++) {
                U8 v = tok->data.ptr[i];
                if (v == 0xa) {
                    printf("\\n");
                } else if (v == '\\') {
                    printf("\\\\");
                } else if (v == 0x1b) {
                    printf("\\x1b");
                } else
                    putchar(v);
            }
            printf("\x1b[0m ");
            break;
    }
}

U0 escape_str(Token* tok) {
    if (tok->type != StrToken) return;
    printf("\"");
    for (U16 i = 0; tok->data.ptr[i]; i++) {
        U8 v = tok->data.ptr[i];
        if (v == 0xa) {
            printf("\\n");
        } else if (v == '\\') {
            printf("\\\\");
        } else if (v == 0x1b) {
            printf("\\x1b");
        } else if (v == '"') {
            printf("\\\"");
        } else
            putchar(v);
    }
    printf("\" ");
}

U0 govnasm_embed(U8* tok) {
    printf("\"");
    for (U16 i = 0; tok[i]; i++) {
        U8 v = tok[i];
        if (v == 0xa) {
            printf("$");
        } else if (v < ' ') {
            printf("^%c", 64+v);
        } else if (v == '$') {
            printf("^$");
        } else if (v == '^') {
            printf("^^");
        } else if (v == '"') {
            printf("^\"");
        } else
            putchar(v);
    }
    printf("^@\" ");
}
