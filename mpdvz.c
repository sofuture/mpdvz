#include <stdio.h>

int main() {
    FILE *ptr_file;
    int BSZ = 1024;
    char buf[BSZ];

    ptr_file = fopen("/tmp/mpd.fifo", "rb");
    if (!ptr_file)
        return 1;

    while (fgets(buf, BSZ, ptr_file) != NULL) {
        for (int c = 0; c < BSZ; c++) {
            int cur = (int)buf[c];
            if (cur >= 0) {
                printf("  %.2X", cur);
            } else {
                printf(" -%.2X", -1 * cur);
            }
            //printf("%.8X ", (int)buf[c]);
            if (c % 4 == 3) printf("  ");
            if (c % 16 == 15) printf("\n");
        }
    }

    fclose(ptr_file);

    return  0;
}
