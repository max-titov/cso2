CC = clang
primes: primes.o main.o
        $(CC) -o primes main.o primes.o

primes.o: primes.h
        $(CC) -c primes.c

main.o: primes.c
        $(CC) -c main.c