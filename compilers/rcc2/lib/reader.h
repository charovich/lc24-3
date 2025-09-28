// Roman C Compiler 2 reader
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

typedef struct {
    FILE* fd;
    U8* file_name;
    U16 line;
    U16 col;
} FileStat;

typedef struct {
    U8 cur;
    U8 fd_i;
    FileStat fd[8];
} Reader;

U0 reader_include(Reader* rd, FILE* fd, U8* file_name) {
    U8 n_idx = rd->fd[0].fd ? ++rd->fd_i : 0;
    FileStat* fs = &(rd->fd[n_idx]);

    fs->fd = fd;
    fs->line = 1;
    fs->col = 0;
    fs->file_name = strdup(file_name);
    // printf("Loading %s\n", file_name);
}

U8 reader_next(Reader* rd) {
    FileStat* fs = &(rd->fd[rd->fd_i]);
    if(!fs->fd || !fread(&rd->cur, 1, 1, fs->fd)) {
        if (fs->fd) { fclose(fs->fd); fs->fd = 0; }
        if (fs->file_name) { free(fs->file_name); fs->file_name = 0; }

        if (rd->fd_i == 0) {
            rd->cur = 0; // EOF
        } else {
            rd->fd_i--;
            rd->cur = '\n';
            // printf("Goodbye!!!!\n");
        }
    }

#ifdef PRINTNOW
    fprintf(stderr, "\x1b[7m%c\x1b[0m", rd->cur);
#endif
    if (rd->cur == '\n') {
        fs->line++;
        fs->col = 0;
    } else {
        fs->col++;
    }
    return rd->cur;
}

// Idk why it is Tokenizer error
// It actually should be here
#define tkerr(tkptr, msg, ...) (fprintf(stderr, "%s:%d:%d: \x1b[31merror: \x1b[0m" msg "\n", tkptr->rd->fd[tkptr->rd->fd_i].file_name, tkptr->rd->fd[tkptr->rd->fd_i].line, tkptr->rd->fd[tkptr->rd->fd_i].col, ## __VA_ARGS__), 1)
