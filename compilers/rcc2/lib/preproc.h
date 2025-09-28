ERR preprocessor(Reader* rd) {
    /* Directive */
    U8 buf[16];
    U8 c = rd->cur, i = 0;
    while (c && c != ' ' && c != '\n') {
        if (i == NAME_LIMIT) return Error;
        buf[i++] = c;
        c = reader_next(rd);
    }
    buf[i] = 0;
    if (!strcmp(buf, "include")) {
        while (c == ' ' || c == '\t') c = reader_next(rd);
        if (c == '"') {
            // Todo: Relative include
            return Error;
        } else if (c == '<') {
            // TODO: -I...
            c = reader_next(rd);
            i = 0;
            while (c && c != '>' && c != '\n') {
                if (i == NAME_LIMIT) return Error;
                buf[i++] = c;
                c = reader_next(rd);
            }
            buf[i] = 0;
            if (c != '>') return Error;
            reader_next(rd);

            FILE* incf = fopen(buf, "rb");
            if (!incf) {
                fprintf(stderr, "[[[[[PREPROC]]]]] %s NOT FOUND\n", buf);
                return Error;
            }
            reader_include(rd, incf, buf);

            // Skip whitespace

            return Ok;
        }
    }
    fprintf(stderr, "[[[[[PREPROC]]]]] INVALID COMMAND %s\n", buf);
    return Error;
}
