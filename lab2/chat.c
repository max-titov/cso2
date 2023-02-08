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

void cleanup()
{
	shm_unlink(inbox_array);
	munmap(inbox, size);
	shm_unlink(outbox_array);
	munmap(outbox, size);
	printf("Cleanup executed\n");
}

void read_inbox()
{
	printf("\n");
	fputs(inbox, stdout);
	printf("\n");
	fflush(stdout);
	inbox[0] = '\0';
}

static void sig_handler(int signum)
{
	if (signum == SIGTERM)
	{
		printf("\nsigterm received\n");
		cleanup();
		exit(0);
	}
	else if (signum == SIGINT)
	{
		printf("\nsigint received\n");
		cleanup();
		kill(otherPid, SIGINT);
		exit(0);
	}
	else if (signum == SIGUSR1)
	{
		read_inbox();
		// printf("\n");
	}
}

void sigaction_setup()
{
	struct sigaction sa;
	sa.sa_handler = &sig_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGUSR1, &sa, NULL);
}

void setup_pids()
{
	myPid = getpid();
	printf("This program's PID: %d\n", myPid);
	printf("Enter the other instances's PID\n");
	scanf("%d", &otherPid);
}

void make_inbox()
{
	snprintf(inbox_array, sizeof inbox_array, "/%d-mmchat", myPid);
	int inbox_fd = shm_open(inbox_array, O_CREAT | O_RDWR, 0666);
	if (inbox_fd < 0)
	{
		printf("%s", "Error making inbox");
	}
	inbox = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, inbox_fd, 0);
	if (inbox == (char *)MAP_FAILED)
	{
		printf("%s", "Error mapping inbox");
	}
	ftruncate(inbox_fd, size);
	close(inbox_fd);

	inbox[0] = '\0';
}

void connect_outbox()
{
	snprintf(outbox_array, sizeof outbox_array, "/%d-mmchat", otherPid);
	int outbox_fd = shm_open(outbox_array, O_CREAT | O_RDWR, 0666);

	if (outbox_fd < 0)
	{
		printf("%s", "Error making outbox");
	}

	outbox = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, outbox_fd, 0);
	if (outbox == (char *)MAP_FAILED)
	{
		printf("%s", "Error mapping outbox");
	}
	ftruncate(outbox_fd, size);
	close(outbox_fd);
}

void run_chat_loop()
{
	while (1)
	{
		printf("%s", "Input Message: ");
		if (scanf("%99s", outbox) == EOF)
		{
			break;
		}
		kill(otherPid, SIGUSR1);
		while (outbox[0])
		{
			usleep(10000);
		}
	}
	kill(myPid, SIGINT);
}

int main()
{
	sigaction_setup();

	setup_pids();
	make_inbox();
	connect_outbox();

	run_chat_loop();

	return 0;
}