#define _XOPEN_SOURCE 700
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/syscall.h>

int size = 4096;
char inbox_array[1024];
char *inbox;
char outbox_array[1024];
char *outbox;

int myPid = 0;
int otherPid = 0;

long long half_sec_nano = 500000000;
long long one_sec_nano = 1000000000;

int signal_received = 0;

long long nsecs()
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	// printf("sec: %lld  nsec: %lld\n", t.tv_sec, t.tv_nsec);
	return t.tv_sec * 1000000000 + t.tv_nsec;
}

long long test_timing_overhead()
{
	// warmup
	for (int i = 0; i < 1000000; i++)
	{
		long long time_before = nsecs();
		long long elapsed_time = nsecs() - time_before;
	}
	int count = 0;
	long long total_elapsed_time = 0;
	while (total_elapsed_time < half_sec_nano)
	{
		count++;
		long long time_before = nsecs();
		long long elapsed_time = nsecs() - time_before;
		total_elapsed_time += elapsed_time;
	}
	long long average = total_elapsed_time / count;
	printf("Timing Overhead: %lld\n", average);
	return average;
}

void empty_func()
{
	return;
}

void pid_func()
{
	myPid = getpid();
}

void system_func()
{
	syscall("/bin/true");
}

static void sig_handler(int signum)
{
	if (signum == SIGQUIT)
	{
		return;
	}
	else if (signum == SIGTERM)
	{
		kill(otherPid, SIGUSR1);
		return;
	}
	else if (signum == SIGUSR1)
	{
		signal_received = 1;
		return;
	}
}

void sigaction_setup()
{
	struct sigaction sa;
	sa.sa_handler = &sig_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGUSR1, &sa, NULL);
}

void setup_pids()
{
	myPid = getpid();
	printf("This program's PID: %d\n", myPid);
	printf("Enter the other instances's PID\n");
	scanf("%d", &otherPid);
}

void signal_curr_func()
{
	kill(myPid, SIGQUIT);
}

void signal_other_func()
{
	kill(otherPid, SIGTERM);
	while (!signal_received)
	{
		usleep(1);
	}
	signal_received = 0;
}

void timing_func(int choice)
{
	long long timing_overhead = test_timing_overhead();

	// warmup
	for (int i = 0; i < 1000000; i++)
	{
		if (choice == 1)
			empty_func();
		else if (choice == 2)
			pid_func();
		else if (choice == 3)
			system_func();
		else if (choice == 4)
			signal_curr_func();
		else if (choice == 5)
			signal_other_func();
	}
	int count = 0;
	long long total_elapsed_time = 0;
	while (total_elapsed_time < half_sec_nano)
	{
		count++;
		long long time_before = nsecs();
		if (choice == 1)
			empty_func();
		else if (choice == 2)
			pid_func();
		else if (choice == 3)
			system_func();
		else if (choice == 4)
			signal_curr_func();
		else if (choice == 5)
			signal_other_func();
		long long elapsed_time = nsecs() - time_before;
		total_elapsed_time += elapsed_time;
	}
	long long average = total_elapsed_time / count - timing_overhead;
	double average_sec = (double)average / one_sec_nano;
	printf("Choice: %d  Number of runs: %d  Average time (ns): %lld  Average time (s): %f\n", choice, count, average, average_sec);
}

void infinite_loop()
{
	while (1)
	{
		usleep(10);
	}
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
	if (choice == 4)
	{
		sigaction_setup();
	}
	if (choice == -1 || choice == 5)
	{
		setup_pids();
		sigaction_setup();
	}
	if (choice == -1)
	{
		infinite_loop();
	}
	timing_func(choice);
	return 0;
}