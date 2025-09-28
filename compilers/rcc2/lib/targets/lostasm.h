U0 lostasm_embed(FILE* out, U8* tok) {
    fprintf(out, "\"");
    for (U16 i = 0; tok[i]; i++) {
        U8 v = tok[i];
        if (v == 0xa)
            fprintf(out, "$");
        else if (v < ' ')
            fprintf(out, "^%c", 64+v);
        else if (v == '$')
            fprintf(out, "^$");
        else if (v == '^')
            fprintf(out, "^^");
        else if (v == '"')
            fprintf(out, "\\\""); // xi wtf
        else
            fprintf(out, "%c", v);
    }
    fprintf(out, "^@\" ");
}
