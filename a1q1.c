#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t pid1, pid2;

    // Parent process forks a child process
    pid1 = fork();

    if (pid1 < 0) {
        // Error handling if fork fails
        perror("Fork failed");
        exit(1);
    }

    if (pid1 == 0) {
        // Child process
        // Child forks a grandchild
        pid2 = fork();

        if (pid2 < 0) {
            // Error handling if fork fails
            perror("Fork failed");
            exit(1);
        }

        if (pid2 == 0) {
            // Grandchild process
            printf("My process id is %d, my parent's id is %d, and my grandparent's id is %d\n", getpid(), getppid(), getppid());
            exit(0);
        } else {
            // Child process (waiting for grandchild to finish)
            wait(NULL);
        }
        exit(0);
    } else {
        // Parent process (waiting for child to finish)
        wait(NULL);
    }

    return 0;
}
