CC = clang
CFLAGS = -Wall
LDFLAGS = -Wall

TARGETS= main.o libmlpt.a pagetable

all: $(TARGETS)

clean:
	rm --force main.o libmlpt.a pagetable

main.o: main.c config.h mlpt.h
	$(CC) -c main.c

libmlpt.a: main.o
	ar rcs libmlpt.a main.o

pagetable: libmlpt.a
	$(CC) -o pagetable libmlpt.a