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
    U8 cur;
    U16 line;
    U16 col;
} Reader;

U0 reader_init(Reader* rd, FILE* fd) {
    rd->line = 1;
    rd->col = 0;
    rd->fd = fd;
}
U8 reader_next(Reader* rd) {
    if(fread(&rd->cur, 1, 1, rd->fd) == 0) {
        rd->cur = 0; // EOF
    }
#ifdef PRINTNOW
    printf("\x1b[7m%c\x1b[0m", rd->cur);
#endif
    if (rd->cur == 0x0a) {
        rd->line++;
        rd->col = 0;
    } else {
        rd->col++;
    }
    return rd->cur;
}
