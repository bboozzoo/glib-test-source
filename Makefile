SRCS = source-thread.c

CFLAGS = -ggdb $(shell pkg-config --cflags glib-2.0 gthread-2.0)
LIBS = $(shell pkg-config --libs glib-2.0 gthread-2.0)

source-thread: $(SRCS:.c=.o)
	$(CC) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) -o $@ -c $^ $(CFLAGS)

clean:
	rm -f *.o source-thread
