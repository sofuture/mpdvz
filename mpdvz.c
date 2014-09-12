#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef __linux__
#include <sys/ioctl.h>
#endif

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

#define SCREEN_BUFFER_SIZE 200
int vals[SCREEN_BUFFER_SIZE];
int start = 0;

void storev(int v){
    vals[start++] = v;
    if(start == SCREEN_BUFFER_SIZE) start = 0;
#ifdef DEBUG
    printf("storing val in buf loc: %d\n", start);
#endif
}

void display(int row, int v) {
    float r = (v + 32767.0) / (32767.0 * 2.0);
    int count = (int) (r * win_width);
    int space = (win_width - count) / 2;
    for(int i = 0; i < space; i++) printf(" ");
    for(int i = 0; i < count; i++) printf("#");
    for(int i = 0; i < space - 1; i++) printf(" ");
    printf("\n\eM");
}

void paint() {
    int height = win_height;
    int mystart = start + SCREEN_BUFFER_SIZE - height;
    int total = 0;


    for(int i = mystart; i < SCREEN_BUFFER_SIZE && total < height; i++){
        display(total++, vals[i]);
    }
    for(int i = 0; i < start && total < height; i++){
        display(total++, vals[i]);
    }

}


int main() {
    getdims();

    FILE *ptr_file;
    int BSZ = 1024;
    short buf[BSZ];

    ptr_file = fopen("/tmp/mpd.fifo", "rb");
    if (!ptr_file)
        return 1;

    while (fread(buf, sizeof(short), BSZ, ptr_file) > 0) {
        for (int c = 0; c < BSZ; c++) {
            int cur = (int)buf[c];
            if(c % 128 == 0) {
                storev(cur);
                if(c % 512 == 0) paint();
            }
        }
    }

    fclose(ptr_file);
    return  0;
}

