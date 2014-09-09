#include <stdio.h>
#include <ncurses.h>

void init(){
    int rows, cols;
    initscr();
    getmaxyx(stdscr, rows, cols);


    refresh();
}

void cleanup() {
    endwin();
}

int main() {
    init();

    FILE *ptr_file;
    int BSZ = 1024;
    short buf[BSZ];

    ptr_file = fopen("/tmp/mpd.fifo", "rb");
    if (!ptr_file)
        return 1;

    while (fread(buf, sizeof(short), BSZ, ptr_file) > 0) {
        for (int c = 0; c < BSZ; c++) {
            short cur = buf[c];
            if (c % 64 == 0)
            printf(" %d", cur);
            if (c % 4 == 3) printf("  ");
            if (c % 16 == 15) printf("\n");
        }
    }

    fclose(ptr_file);

    cleanup();
    return  0;
}

