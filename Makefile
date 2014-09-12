CFLAGS += -std=c11 -Wall -O2
TBCFLAGS = -c -Wall -O2
LIBS +=

OBJS =

.PHONY: all
all: mpdvz mpdvz_debug

mpdvz: mpdvz.c ${OBJS}
	$(CC) -o $@ $< $(CFLAGS) $(OBJS) $(LIBS)

mpdvz_debug: mpdvz.c ${OBJS}
	$(CC) -o $@ mpdvz.c $(CFLAGS) $(OBJS) $(LIBS) -DDEBUG

clean:
	rm -rf mpdvz mpdvz_debug *.o

