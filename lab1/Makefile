CC = clang
CFLAGS = -Wall
LDFLAGS = -Wall

TARGETS= guesser libsay.a say.h

.PHONY: all clean

all: $(TARGETS)

clean:
	rm --force grunt.o cheer.o guesser.o libsay.a guesser

grunt.o: grunt.c say.h
	$(CC) -c grunt.c

cheer.o: cheer.c say.h
	$(CC) -c cheer.c

guesser.o: guesser.c say.h
	$(CC) -c guesser.c

libsay.a: cheer.o grunt.o
	ar rcs libsay.a cheer.o grunt.o

guesser: libsay.a guesser.o say.h
	$(CC) -o guesser guesser.o libsay.a