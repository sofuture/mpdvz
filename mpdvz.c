#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#ifdef __linux__
#include <sys/ioctl.h>
#endif

#include <ncurses.h>


int win_width, win_height, win_mode;

static int _getdims(int fd) {
#ifdef TIOCGSIZE
    struct ttysize ts;
    if(ioctl(fd, TIOCGSIZE, &ts)) { perror(""); return -1; }
    if(ts.ts_cols > 0) {
        win_width = ts.ts_cols;
        win_height = ts.ts_lines;
        win_mode = 1 + fd;
        return 0;
    }
#elif defined(TIOCGWINSZ)
    struct winsize ws;
    if(ioctl(fd, TIOCGWINSZ, &ws)) { perror(""); return -1; }
    if(ws.ws_col > 0) {
        win_width = ws.ws_col;
        win_height = ws.ws_row;
        win_mode = 1 + fd;
        return 0;
    }
#endif
    return -1;
}

static void getdims() {
    int retval, rows, cols;
    FILE *f;
    win_mode = 0;
    retval = 0;
#define GETCOLS(x) \
    if(isatty(x) && ((retval = _getdims(x)) != -1)) return;
    GETCOLS(STDIN_FILENO)
    GETCOLS(STDOUT_FILENO)
    GETCOLS(STDERR_FILENO)
    if(getenv("COLUMNS") && getenv("LINES")) {
        win_width = atoi(getenv("COLUMNS"));
        win_height= atoi(getenv("LINES"));
        if(win_width > 0 && win_height > 0) { win_mode = 4; return; }
    }
#define STTY(x, y) \
    if((f = popen("stty " x " size", "r"))!=NULL) { \
        if(fscanf(f, "%d %d", &rows, &cols) > 0 && cols>0) \
            { win_height = rows; win_width = cols; win_mode = y; pclose(f); return; } pclose(f);\
    }
    STTY("-f /dev/stderr", 5)
    STTY("", 6)
    STTY("-f /dev/stdout", 7)
    win_width = win_height = 0;
    win_mode = 8;
    return;
}

typedef struct {
    int size;
    int start;
    int end;
    int *buf;
} CBuf;


void cb_init(CBuf *buf, int size) {
    buf->size = size + 1;
    buf->start = 0;
    buf->end = 0;
    buf->buf = calloc(buf->size, sizeof(int));
}

void cb_write(CBuf *buf, int val) {
    buf->buf[buf->end] = val;
    buf->end = (buf->end + 1) % buf->size;
    if(buf->end == buf->start) {
        buf->start = (buf->start + 1) % buf->size;
    }
}

int cb_read(CBuf *buf, int idx) {
    return buf->buf[(buf->start + idx) % buf->size];
}

void cb_debug(CBuf *buf) {
    for(int i = 0; i < buf->size - 1; i++) {
        int v = cb_read(buf, i);
        float r = (v + 32767.0) / (32767.0 * 2.0);
        printf("%f\n", r);
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

void display(int v) {
    float r = (v + 32767.0) / (32767.0 * 2.0);
    int count = (int) (r * win_width);
    for(int i = 0; i < count; i++){
        printf("#");
    }
    printf("\n");
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
            int cur = (int)buf[c];
            if(c % 128 == 0) {
                //cb_write(&cb, (int)cur);
                display(cur);
            }
        }
        //cb_debug(&cb);
    }

    fclose(ptr_file);
    return  0;
}

int main() {
    //init();
    getdims();
    go();
    //cleanup();
}

