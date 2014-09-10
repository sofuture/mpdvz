#include <stdio.h>
#include <stdlib.h>

#include <ncurses.h>


typedef struct {
    int size;
    int start;
    int end;
    short *buf;
} CBuf;


void cb_init(CBuf *buf, int size) {
    buf->size = size + 1;
    buf->start = 0;
    buf->end = 0;
    buf->buf = calloc(buf->size, sizeof(short));
}

void cb_write(CBuf *buf, short val) {
    buf->buf[buf->end] = val;
    buf->end = (buf->end + 1) % buf->size;
    if(buf->end == buf->start) {
        buf->start = (buf->start + 1) % buf->size;
    }
}

short cb_read(CBuf *buf, int idx) {
    return buf->buf[(buf->start + idx) % buf->size];
}

void cb_debug(CBuf *buf) {
    for(int i = 0; i < buf->size - 1; i++) {
        printf("%d\n", cb_read(buf, i));
    }
}

void init() {
    int rows, cols;
    initscr();
    getmaxyx(stdscr, rows, cols);
    refresh();
}

void cleanup() {
    endwin();
}

int go() {
    FILE *ptr_file;
    int BSZ = 1024;
    short buf[BSZ];

    CBuf cb;
    cb_init(&cb, 100);

    ptr_file = fopen("/tmp/mpd.fifo", "rb");
    if (!ptr_file)
        return 1;

    while (fread(buf, sizeof(short), BSZ, ptr_file) > 0) {
        for (int c = 0; c < BSZ; c++) {
            short cur = buf[c];
            cb_write(&cb, cur);
        }
        cb_debug(&cb);
    }

    fclose(ptr_file);
    return  0;
}

int main() {
    go();
}

