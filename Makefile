FLAGS = -std=c89 -pedantic -Wall -Wextra -Wl,-rpath='${ORIGIN},${ORIGIN}/radix'
CFLAGS = -fPIC -Ofast -g -march=core2 -mtune=generic
LDFLAGS = -shared

SRC = ${wildcard src/*.c}
OBJ = ${SRC:.c=.o}

all: libsurrender.so a.out

libsurrender.so: ${OBJ}
	${CC} -o $@ $^ ${LDFLAGS}

libsurrender.a: ${OBJ}
	ar rcs $@ $^

a.out: main.c libsurrender.a radix/libradix.a
	${CC} $^ -lSDL2

clean:
	rm -f a.out libsurrender.* ${OBJ}

.PHONY: all clean
