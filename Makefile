FLAGS := -std=gnu17 -pedantic -Wall -Wextra -Wl,-rpath='$$ORIGIN',-rpath='$$ORIGIN/radix' -lm -Wl,--allow-multiple-definition
CFLAGS := ${FLAGS} -fPIC -Ofast -g -march=core2 -mtune=generic
LDFLAGS := ${CFLAGS} -shared

SRC = ${wildcard src/*.c}
OBJ = ${SRC:.c=.o}

all: libsurrender.so a.out

libsurrender.so: ${OBJ}
	${CC} -o $@ $^ ${LDFLAGS}

libsurrender.a: ${OBJ}
	ar rcs $@ $^

a.out: main.c libsurrender.a radix/libradix.a
	${CC} $^ -lSDL2 ${CFLAGS}

clean:
	rm -f a.out libsurrender.* ${OBJ}

.PHONY: all clean
