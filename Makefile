CFLAGS += -std=c11 -Wall -pedantic -O2
LIBS +=

OBJS = ll.o termbox/src/termbox.o termbox/src/utf8.o

.PHONY: all
all: mpdvz mpdvz_debug

mpdvz: mpdvz.c ${OBJS}
	$(CC) -o $@ $< $(CFLAGS) $(OBJS) $(LIBS)

mpdvz_debug: mpdvz.c ${OBJS}
	$(CC) -o $@ mpdvz.c $(CFLAGS) $(OBJS) $(LIBS) -DDEBUG

clean:
	rm -rf mpdvz mpdvz_debug *.o termbox/src/*.o

