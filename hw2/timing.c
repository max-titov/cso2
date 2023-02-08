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

long long half_sec_nano = 500000000;
long long one_sec_nano = 1000000000;

long long nsecs()
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	// printf("sec: %lld  nsec: %lld\n", t.tv_sec, t.tv_nsec);
	return t.tv_sec * 1000000000 + t.tv_nsec;
}

long long test_timing_overhead()
{
	int count = 100;
	long long total_elapsed_time = 0;
	for (int i = 0; i < count; i++)
	{
		long long time_before = nsecs();
		long long elapsed_time = nsecs() - time_before;
		total_elapsed_time += elapsed_time;
	}
	return total_elapsed_time / count;
}

void empty_func()
{
	return;
}

void timing_func(int choice)
{
	long long timing_overhead = test_timing_overhead();
	int count = 0;
	long long total_elapsed_time = 0;
	while (total_elapsed_time < half_sec_nano)
	{
		count++;
		long long time_before = nsecs();
		if (choice == 1)
			empty_func();

		long long elapsed_time = nsecs() - time_before;
		total_elapsed_time += elapsed_time;
	}
	total_elapsed_time -= timing_overhead * count; // adjust for timing overhead
	long long average = total_elapsed_time / count;
	double double average_sec = (double)average / one_sec_nano;
	printf("Choice: %d Number of runs: %d Average time (s): %f\n", choice, count, average_sec);
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