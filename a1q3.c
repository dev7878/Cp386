#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pipe1[2], pipe2[2];

    // Create first pipe (for ps -> sort)
    if (pipe(pipe1) == -1) {
        perror("pipe1 failed");
        exit(1);
    }

    // Fork the first child to run "ps -e -o sid"
    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("fork1 failed");
        exit(1);
    }

    if (pid1 == 0) {
        // First child (ps -e -o sid)
        // Redirect stdout to the write end of pipe1
        dup2(pipe1[1], STDOUT_FILENO);
        // Redirect stderr to /dev/null to suppress unwanted output
        freopen("/dev/null", "w", stderr);
        // Close unused pipe ends
        close(pipe1[0]);
        close(pipe1[1]);
        // Execute "ps -e -o sid"
        execlp("ps", "ps", "-e", "-o", "sid", NULL);
        perror("execlp ps failed");
        exit(1);
    }

    // Create second pipe (for sort -> wc)
    if (pipe(pipe2) == -1) {
        perror("pipe2 failed");
        exit(1);
    }

    // Fork the second child to run "sort -u"
    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("fork2 failed");
        exit(1);
    }

    if (pid2 == 0) {
        // Second child (sort -u)
        // Redirect stdin to the read end of pipe1
        dup2(pipe1[0], STDIN_FILENO);
        // Redirect stdout to the write end of pipe2
        dup2(pipe2[1], STDOUT_FILENO);
        // Close unused pipe ends
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        // Execute "sort -u"
        execlp("sort", "sort", "-u", NULL);
        perror("execlp sort failed");
        exit(1);
    }

    // Close pipe1 (ps -> sort) as it is no longer needed
    close(pipe1[0]);
    close(pipe1[1]);

    // Fork the third child to run "wc -l"
    pid_t pid3 = fork();
    if (pid3 < 0) {
        perror("fork3 failed");
        exit(1);
    }

    if (pid3 == 0) {
        // Third child (wc -l)
        // Redirect stdin to the read end of pipe2
        dup2(pipe2[0], STDIN_FILENO);
        // Close unused pipe ends
        close(pipe2[0]);
        close(pipe2[1]);
        // Execute "wc -l"
        execlp("wc", "wc", "-l", NULL);
        perror("execlp wc failed");
        exit(1);
    }

    // Close pipe2 (sort -> wc) as it is no longer needed
    close(pipe2[0]);
    close(pipe2[1]);

    // Wait for all child processes to finish
    wait(NULL);
    wait(NULL);
    wait(NULL);

    return 0;
}
