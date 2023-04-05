#define _XOPEN_SOURCE 700

#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

char *getoutput(const char *command) {
	int pipefd[2];
	pipe(pipefd);

	// Fork the current process to create a child process
	pid_t fork_pid = fork();

	int child = fork_pid <= 0;
	if (child) {
		// Redirect the standard output of the child process to the write end of the pipe
		dup2(pipefd[1], STDOUT_FILENO);

		// Close the read end of the pipe
		close(pipefd[0]);
		close(pipefd[1]);

		// Execute the shell command in the child process
		execl("/bin/sh", "sh", "-c", command, NULL);
		_exit(127);
		return NULL;
	} else {
		// Allocate memory for the output string
		char *out = (char *)malloc(1024);
		size_t length = 0;
		close(pipefd[1]);
		FILE *temp_file = fdopen(pipefd[0], "r");

		getdelim(&out, &length, '\0', temp_file);

		// Close the stream and the read end of the pipe
		close(pipefd[0]);
		fclose(temp_file);

		// Wait for the child process to exit
		waitpid(fork_pid, NULL, 0);
		return out;
	}
}

char *parallelgetoutput(int count, const char **argv_base) {
	// Create a pipe for communication between the parent and child processes
	int pipefd[2];
	pipe(pipefd);

	for (int k = 0; k < count; k++) {
		// Fork the current process to create a child process
		pid_t fork_pid = fork();
		if (fork_pid == 0) {
			// Create a string representation of the current instance index
			char last_argument[16];
			snprintf(last_argument, sizeof(last_argument), "%d", k);

			// Modify the command arguments to include the current instance index
			char **var = argv_base;
			while (*var != NULL) {
				var++;
			}
			*var = last_argument;
			var++;
			*var = NULL;

			// Redirect the standard output of the child process to the write end of the pipe
			dup2(pipefd[1], STDOUT_FILENO);

			// Close the read and write ends of the pipe
			close(pipefd[0]);
			close(pipefd[1]);
			execv(argv_base[0], argv_base);
			_exit(127);
			return NULL;
		}
	}
	// Allocate memory for the output string
	char *out = (char *)malloc(1024);
	size_t length = 0;

	// Wait for all child processes to complete before reading their output
	waitpid(-1, NULL, 0);
	close(pipefd[1]);

	// Read the output of the child processes from the pipe
	FILE *temp_file = fdopen(pipefd[0], "r");
	getdelim(&out, &length, '\0', temp_file);
	fclose(temp_file);
	close(pipefd[0]);

	return out;
}