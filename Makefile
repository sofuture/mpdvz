CFLAGS += -std=c11 -Wall -pedantic -O2
TBCFLAGS = -c -Wall -pedantic -O2
LIBS +=

OBJS = ll.o termbox.o utf8.o

.PHONY: all
all: mpdvz mpdvz_debug

termbox.o: termbox/src/termbox.c termbox/src/utf8.c
	$(CC) $< -c

utf8.o: termbox/src/utf8.c
	$(CC) $< -c

mpdvz: mpdvz.c ${OBJS}
	$(CC) -o $@ $< $(CFLAGS) $(OBJS) $(LIBS)

mpdvz_debug: mpdvz.c ${OBJS}
	$(CC) -o $@ mpdvz.c $(CFLAGS) $(OBJS) $(LIBS) -DDEBUG

clean:
	rm -rf mpdvz mpdvz_debug *.o termbox/src/*.o

