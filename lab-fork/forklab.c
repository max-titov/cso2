#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int my_system(const char *command) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    } else if (pid == 0) {
        // Child process
        char *const args[] = {"/sh", "-c", (char *) command, NULL};
        execv("/bin/sh", args);
        _exit(127);  // exec failed
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        return status;
    }
}

int main(int argc, const char *argv[]) {
    int a1 = my_system("sleep 1; echo hi");
    int a2 = my_system("echo bye");
    int a3 = my_system("flibbertigibbet 23");
    printf("%d %d %d\n", 
        WEXITSTATUS(a1), WEXITSTATUS(a2), WEXITSTATUS(a3));
	printf("\n");

	my_system("echo -n 'type something: ';"
        " read got;"
        " echo Thanks for typing \\\"\"$got\"\\\"");
}