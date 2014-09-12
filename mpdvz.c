#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#ifdef __linux__
#include <sys/ioctl.h>
#endif

char shutdown = 0;
int win_width, win_height, win_mode;
FILE *ptr_file;

static int _getdims(int fd) {
#ifdef TIOCGSIZE
    struct ttysize ts;
    if(ioctl(fd, TIOCGSIZE, &ts)) {
        perror("");
        return -1;
    }
    if(ts.ts_cols > 0) {
        win_width = ts.ts_cols;
        win_height = ts.ts_lines;
        win_mode = 1 + fd;
        return 0;
    }
#elif defined(TIOCGWINSZ)
    struct winsize ws;
    if(ioctl(fd, TIOCGWINSZ, &ws)) {
        perror("");
        return -1;
    }
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
        if(win_width > 0 && win_height > 0) {
            win_mode = 4;
            return;
        }
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

#define SCREEN_BUFFER_SIZE 200
int vals[SCREEN_BUFFER_SIZE];
int start = 0;

void storev(int v) {
    vals[start++] = v;
    if(start == SCREEN_BUFFER_SIZE) start = 0;
#ifdef DEBUG
    printf("storing val in buf loc: %d\n", start);
#endif
}

void display(int v) {
    float r = (v + 32767.0) / (32767.0 * 2.0);
    int count = (int) (r * win_width);
    int space = (win_width - count) / 2;
    for(int i = 0; i < space; i++) printf(" ");
    for(int i = 0; i < count; i++) printf("#");
    printf("\n\eM\eM");
}

int show_usage() {
    printf("Usage: mpdvz [-h|--help] [mpd fifo path]\n\n");
    printf("  path defaults to /tmp/mpd.fifo\n");
    return 0;
}

void handle_resize(int signal) {
    getdims();
}

int cleanup() {
    fclose(ptr_file);
    return 0;
}

void handle_sigint(int signal) {
    shutdown = 1;
}

int main(int argc, char *argv[]) {
    int BSZ = 1024;
    short buf[BSZ];
    char *filename = NULL;

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (strncmp(argv[i], "--help", 6) == 0 ||
                    strncmp(argv[i], "-h", 2) == 0) {
                return show_usage();
            } else {
                filename = argv[i];
            }
        }
    }

    if(filename == NULL) {
        filename = "/tmp/mpd.fifo";
    }

    getdims();

    signal(SIGWINCH, handle_resize);
    signal(SIGINT, handle_sigint);

    ptr_file = fopen(filename, "rb");
    if (!ptr_file) {
        printf("error: can't open %s\n", filename);
        return 1;
    }

    while (!shutdown && fread(buf, sizeof(short), BSZ, ptr_file) > 0) {
        for (int c = 0; c < BSZ; c++) {
            int cur = (int)buf[c];
            if(c % 128 == 0) {
                display(cur);
            }
        }
    }

    return cleanup();
}

