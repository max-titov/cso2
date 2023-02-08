#define _XOPEN_SOURCE 700
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

int size = 4096;
char inbox_array[1024];
char *inbox;
char outbox_array[1024];
char *outbox;

int myPid = 0;
int otherPid = 0;

long long nsecs()
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	printf("sec: %lu  nsec: %lu\n", t.tv_sec, t.tv_nsec);
	return t.tv_sec * 1000000000 + t.tv_nsec;
}

void time_empty_func()
{
}

void timing_func(int choice)
{
	long long time_before = nsecs();
	if (choice == 1)
	{
		time_empty_func();
	}
	long long time_after = nsecs();
	long long elapsed_time = time_after - time_before;
	printf("Elapsed Time: %lu", elapsed_time);
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Please provide an argument (-1,1,2,3,4,5)\n");
		printf("Terminating\n");
		return 0;
	}
	char *p;
	int choice = strtol(argv[1], &p, 10);
	timing_func(choice);
	return 0;
}