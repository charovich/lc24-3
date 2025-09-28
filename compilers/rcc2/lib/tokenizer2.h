typedef enum {
    EOFToken, // 00
    NameToken, // 01
    NumberToken, // 02
    StrToken, // 03
    PlusPlus, // 04
    MinusMinus, // 05
    LeftParen, // 06
    RightParen, // 07
    LeftBracket, // 08
    RightBracket, // 09
    Dot, // 0A
    Arrow, // 0B
    Not, // 0C
    Tilde, // 0D
    Star, // 0E
    Div, // 0F
    Mod, // 10
    Plus, // 11
    Minus, // 12
    LeftShift, // 13
    RightShift, // 14
    Less, // 15
    LessEqual, // 16
    Greater, // 17
    GreaterEqual, // 18
    Equal, // 19
    NotEqual, // 1A
    And, // 1B
    Caret, // 1C
    Or, // 1D
    AndAnd, // 1E
    OrOr, // 1F
    Question, // 20
    Colon, // 21
    Assign, // 22
    StarAssign, // 23
    DivAssign, // 24
    ModAssign, // 25
    PlusAssign, // 26
    MinusAssign, // 27
    LeftShiftAssign, // 28
    RightShiftAssign, // 29
    AndAssign, // 2A
    XorAssign, // 2B
    OrAssign, // 2C
    Comma, // 2D
    Semi, // 2E
    DoubleDot, // 2F
    TripleDot, // 30
    LeftBrace, // 31
    RightBrace, // 32
    IfKeywordToken,
    ElseKeywordToken,
    WhileKeywordToken,
    ForKeywordToken,
    TypedefKeywordToken,
    StructKeywordToken,
    UnionKeywordToken,
    EnumKeywordToken,
    ReturnKeywordToken
} TokenType;
typedef struct {
    TokenType type;
    union {
        U32 number;
        I8 str[NAME_LIMIT+1];
        U8* ptr;
        ERR err;
    };
 } Token;

typedef struct {
    Reader* rd;
    Token cur;
} Tokenizer;

#define getk(tkptr, tktype) if (tkptr->cur.type != tktype) { tkerr(tkptr, "Expected " #tktype); return Error; }
#define decx(c) (c >= 'A' ? 9 + (c & 159) : c - '0')
U0 tokenizer_next(Tokenizer* tok);
U0 tok_fail(Tokenizer* tok, I8* err) {
    tkerr(tok, "%s", err);
    tok->cur.type = EOFToken;
    tok->cur.err = Error;
}
U0 tok_unk(Tokenizer* tok) {
    return tok_fail(tok, "Unknown character");
}
U8 read_escape(Reader* rd, U8* out, U8 stop) {
    reader_next(rd);
    if (!rd->cur) {
        *out = 0;
        return 0;
    } else if (rd->cur == '\\') {
        reader_next(rd);
        if (rd->cur == 'x') {
            reader_next(rd);
            if (!isxdigit(rd->cur)) {
                *out = 0;
                return 0;
            }
            U8 c = decx(rd->cur) * 16;
            reader_next(rd);
            if (!isxdigit(rd->cur)) {
                *out = 0;
                return 0;
            }
            c += decx(rd->cur);
            *out = c;
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
U8* keywords[] = {"if", "else", "while", "for", "typedef", "struct", "union", "enum", "return", 0};
U0 tok_var(Tokenizer* tok) {
    U8 c = tok->rd->cur, i = 0;
    while (c == '_' || isalnum(c)) {
        if (i == NAME_LIMIT) return tok_fail(tok, "Variable limit reached");        tok->cur.str[i++] = c;
        c = reader_next(tok->rd);
    }
    tok->cur.str[i] = 0;
    for(i = 0; keywords[i]; i++) {
        if(!strcmp(keywords[i], tok->cur.str)) {
            tok->cur.type = IfKeywordToken + i;
            return;        }
    }
    tok->cur.type = NameToken;
}
U0 tok_str(Tokenizer* tok) {
    U8* m = malloc(256);
    U8 i = 0;
    while (read_escape(tok->rd, &(m[i++]), '"')) ;
    if (tok->rd->cur != '"') return tok_fail(tok, "String not finished");
    reader_next(tok->rd);
    tok->cur.type = StrToken;
    tok->cur.ptr = m;
}
U0 tok_num(Tokenizer* tok) {
    U32 i = 0;
    while (isdigit(tok->rd->cur)) {
        i = i * 10 + (tok->rd->cur - '0');
        reader_next(tok->rd);
    }
    tok->cur.type = NumberToken;
    tok->cur.number = i;
}
U0 tok_eof(Tokenizer* tok) {
    tok->cur.type = EOFToken;
    tok->cur.err = Ok;
}
U0 tok_on_21(Tokenizer* tok) { // '!'
    U8 cur = reader_next(tok->rd);
    if (cur == '=') {
        reader_next(tok->rd);
        tok->cur.type = NotEqual;
        return;
    }
    tok->cur.type = Not;
}
U0 tok_on_23(Tokenizer* tok) { // '#'
    reader_next(tok->rd);
    return preprocessor(tok->rd) == Ok ? tokenizer_next(tok) : tok_fail(tok, "Preprocessor error");
}
U0 tok_on_25(Tokenizer* tok) { // '%'
    U8 cur = reader_next(tok->rd);
    if (cur == '=') {
        reader_next(tok->rd);
        tok->cur.type = ModAssign;
        return;
    }
    tok->cur.type = Mod;
}
U0 tok_on_26(Tokenizer* tok) { // '&'
    U8 cur = reader_next(tok->rd);
    if (cur == '&') {
        reader_next(tok->rd);
        tok->cur.type = AndAnd;
        return;
    }
    if (cur == '=') {
        reader_next(tok->rd);
        tok->cur.type = AndAssign;
        return;
    }
    tok->cur.type = And;
}
U0 tok_on_27(Tokenizer* tok) { // '''
    U8 c;
    if (!read_escape(tok->rd, &c, '\'')) {
        return tok_fail(tok, "0 chars provided");
    }
    reader_next(tok->rd);
    if (tok->rd->cur != '\'') {
        return tok_fail(tok, "Too many chars provided");
    }
    reader_next(tok->rd);
    tok->cur.type = NumberToken;
    tok->cur.number = c;
}
U0 tok_on_28(Tokenizer* tok) { // '('
    reader_next(tok->rd);
    tok->cur.type = LeftParen;
}
U0 tok_on_29(Tokenizer* tok) { // ')'
    reader_next(tok->rd);
    tok->cur.type = RightParen;
}
U0 tok_on_2a(Tokenizer* tok) { // '*'
    U8 cur = reader_next(tok->rd);
    if (cur == '=') {
        reader_next(tok->rd);
        tok->cur.type = StarAssign;
        return;
    }
    tok->cur.type = Star;
}
U0 tok_on_2b(Tokenizer* tok) { // '+'
    U8 cur = reader_next(tok->rd);
    if (cur == '+') {
        reader_next(tok->rd);
        tok->cur.type = PlusPlus;
        return;
    }
    if (cur == '=') {
        reader_next(tok->rd);
        tok->cur.type = PlusAssign;
        return;
    }
    tok->cur.type = Plus;
}
U0 tok_on_2c(Tokenizer* tok) { // ','
    reader_next(tok->rd);
    tok->cur.type = Comma;
}
U0 tok_on_2d(Tokenizer* tok) { // '-'
    U8 cur = reader_next(tok->rd);
    if (cur == '-') {
        reader_next(tok->rd);
        tok->cur.type = MinusMinus;
        return;
    }
    if (cur == '=') {
        reader_next(tok->rd);
        tok->cur.type = MinusAssign;
        return;
    }
    if (cur == '>') {
        reader_next(tok->rd);
        tok->cur.type = Arrow;
        return;
    }
    tok->cur.type = Minus;
}
U0 tok_on_2e(Tokenizer* tok) { // '.'
    U8 cur = reader_next(tok->rd);
    if (cur == '.') {
        cur = reader_next(tok->rd);
        if (cur == '.') {
            reader_next(tok->rd);
            tok->cur.type = TripleDot;
            return;
        }
        tok->cur.type = DoubleDot;
        return;
    }
    tok->cur.type = Dot;
}
U0 tok_on_2f(Tokenizer* tok) { // '/'
    U8 cur = reader_next(tok->rd);
    if (cur == '*') {
        return;
    }
    if (cur == '/') {
        reader_next(tok->rd);
        while(tok->rd->cur && tok->rd->cur != '\n')reader_next(tok->rd);
        return tokenizer_next(tok);
    }
    if (cur == '=') {
        reader_next(tok->rd);
        tok->cur.type = DivAssign;
        return;
    }
    tok->cur.type = Div;
}
U0 tok_on_30(Tokenizer* tok) { // '0'
    reader_next(tok->rd);switch (tok->rd->cur) {    case 'x': {
            reader_next(tok->rd);
            if (!isxdigit(tok->rd->cur)) {
                return tok_fail(tok, "Zero-length hex");        }
            U32 c = 0;        while (isxdigit(tok->rd->cur)) {
                c = c * 16 + decx(tok->rd->cur);
                reader_next(tok->rd);
            }
            tok->cur.type = NumberToken;
            tok->cur.number = c;
            break;    }
        default:
            tok->cur.type = NumberToken;
            tok->cur.number = 0;
    }
}
U0 tok_on_3a(Tokenizer* tok) { // ':'
    reader_next(tok->rd);
    tok->cur.type = Colon;
}
U0 tok_on_3b(Tokenizer* tok) { // ';'
    reader_next(tok->rd);
    tok->cur.type = Semi;
}
U0 tok_on_3c(Tokenizer* tok) { // '<'
    U8 cur = reader_next(tok->rd);
    if (cur == '<') {
        cur = reader_next(tok->rd);
        if (cur == '=') {
            reader_next(tok->rd);
            tok->cur.type = LeftShiftAssign;
            return;
        }
        tok->cur.type = LeftShift;
        return;
    }
    if (cur == '=') {
        reader_next(tok->rd);
        tok->cur.type = LessEqual;
        return;
    }
    tok->cur.type = Less;
}
U0 tok_on_3d(Tokenizer* tok) { // '='
    U8 cur = reader_next(tok->rd);
    if (cur == '=') {
        reader_next(tok->rd);
        tok->cur.type = Equal;
        return;
    }
    tok->cur.type = Assign;
}
U0 tok_on_3e(Tokenizer* tok) { // '>'
    U8 cur = reader_next(tok->rd);
    if (cur == '=') {
        reader_next(tok->rd);
        tok->cur.type = GreaterEqual;
        return;
    }
    if (cur == '>') {
        cur = reader_next(tok->rd);
        if (cur == '=') {
            reader_next(tok->rd);
            tok->cur.type = RightShiftAssign;
            return;
        }
        tok->cur.type = RightShift;
        return;
    }
    tok->cur.type = Greater;
}
U0 tok_on_3f(Tokenizer* tok) { // '?'
    reader_next(tok->rd);
    tok->cur.type = Question;
}
U0 tok_on_5b(Tokenizer* tok) { // '['
    reader_next(tok->rd);
    tok->cur.type = LeftBracket;
}
U0 tok_on_5d(Tokenizer* tok) { // ']'
    reader_next(tok->rd);
    tok->cur.type = RightBracket;
}
U0 tok_on_5e(Tokenizer* tok) { // '^'
    U8 cur = reader_next(tok->rd);
    if (cur == '=') {
        reader_next(tok->rd);
        tok->cur.type = XorAssign;
        return;
    }
    tok->cur.type = Caret;
}
U0 tok_on_7b(Tokenizer* tok) { // '{'
    reader_next(tok->rd);
    tok->cur.type = LeftBrace;
}
U0 tok_on_7c(Tokenizer* tok) { // '|'
    U8 cur = reader_next(tok->rd);
    if (cur == '=') {
        reader_next(tok->rd);
        tok->cur.type = OrAssign;
        return;
    }
    if (cur == '|') {
        reader_next(tok->rd);
        tok->cur.type = OrOr;
        return;
    }
    tok->cur.type = Or;
}
U0 tok_on_7d(Tokenizer* tok) { // '}'
    reader_next(tok->rd);
    tok->cur.type = RightBrace;
}
U0 tok_on_7e(Tokenizer* tok) { // '~'
    reader_next(tok->rd);
    tok->cur.type = Tilde;
}
typedef U0 (*TokNxFn)(Tokenizer* tok);
TokNxFn nxfns[128] = {
&tok_eof,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   
&tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   &tok_unk,   
&tok_unk,   &tok_on_21, &tok_str,   &tok_on_23, &tok_unk,   &tok_on_25, &tok_on_26, &tok_on_27, &tok_on_28, &tok_on_29, &tok_on_2a, &tok_on_2b, &tok_on_2c, &tok_on_2d, &tok_on_2e, &tok_on_2f, 
&tok_on_30, &tok_num,   &tok_num,   &tok_num,   &tok_num,   &tok_num,   &tok_num,   &tok_num,   &tok_num,   &tok_num,   &tok_on_3a, &tok_on_3b, &tok_on_3c, &tok_on_3d, &tok_on_3e, &tok_on_3f, 
&tok_unk,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   
&tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_on_5b, &tok_unk,   &tok_on_5d, &tok_on_5e, &tok_var,   
&tok_unk,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   
&tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_var,   &tok_on_7b, &tok_on_7c, &tok_on_7d, &tok_on_7e, &tok_unk,   
};
U0 tokenizer_next(Tokenizer* tok) {
    U8 c = tok->rd->cur;
    while (isspace(c)) c = reader_next(tok->rd);
    return nxfns[c>127?'_':c](tok);
}
U8* token_name(Token* tok) {
    switch (tok->type) {
        case EOFToken: return "EOF";
        case IfKeywordToken: return "if";
        case WhileKeywordToken: return "while";
        case ForKeywordToken: return "for";
        case NameToken: return "<name>";
        case NumberToken: return "<int>";
        case StrToken: return "<str>";
        case PlusPlus: return "PlusPlus\"++\""; 
        case MinusMinus: return "MinusMinus\"--\""; 
        case LeftParen: return "LeftParen\"(\""; 
        case RightParen: return "RightParen\")\""; 
        case LeftBracket: return "LeftBracket\"[\""; 
        case RightBracket: return "RightBracket\"]\""; 
        case Dot: return "Dot\".\""; 
        case Arrow: return "Arrow\"->\""; 
        case Not: return "Not\"!\""; 
        case Tilde: return "Tilde\"~\""; 
        case Star: return "Star\"*\""; 
        case Div: return "Div\"/\""; 
        case Mod: return "Mod\"%\""; 
        case Plus: return "Plus\"+\""; 
        case Minus: return "Minus\"-\""; 
        case LeftShift: return "LeftShift\"<<\""; 
        case RightShift: return "RightShift\">>\""; 
        case Less: return "Less\"<\""; 
        case LessEqual: return "LessEqual\"<=\""; 
        case Greater: return "Greater\">\""; 
        case GreaterEqual: return "GreaterEqual\">=\""; 
        case Equal: return "Equal\"==\""; 
        case NotEqual: return "NotEqual\"!=\""; 
        case And: return "And\"&\""; 
        case Caret: return "Caret\"^\""; 
        case Or: return "Or\"|\""; 
        case AndAnd: return "AndAnd\"&&\""; 
        case OrOr: return "OrOr\"||\""; 
        case Question: return "Question\"?\""; 
        case Colon: return "Colon\":\""; 
        case Assign: return "Assign\"=\""; 
        case StarAssign: return "StarAssign\"*=\""; 
        case DivAssign: return "DivAssign\"/=\""; 
        case ModAssign: return "ModAssign\"%=\""; 
        case PlusAssign: return "PlusAssign\"+=\""; 
        case MinusAssign: return "MinusAssign\"-=\""; 
        case LeftShiftAssign: return "LeftShiftAssign\"<<=\""; 
        case RightShiftAssign: return "RightShiftAssign\">>=\""; 
        case AndAssign: return "AndAssign\"&=\""; 
        case XorAssign: return "XorAssign\"^=\""; 
        case OrAssign: return "OrAssign\"|=\""; 
        case Comma: return "Comma\",\""; 
        case Semi: return "Semi\";\""; 
        case DoubleDot: return "DoubleDot\"..\""; 
        case TripleDot: return "TripleDot\"...\""; 
        case LeftBrace: return "LeftBrace\"{\""; 
        case RightBrace: return "RightBrace\"}\""; 
    }
    return "???";
}
