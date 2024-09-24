#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    // Hardcoded input file
    const char *input_file = "input";  // Change this to the name of your input file

    // Open the input file
    FILE *infile = fopen(input_file, "r");
    if (!infile) {
        perror("Error opening file");
        return 1;
    }

    int n; // Number of programs
    fscanf(infile, "%d", &n); // Read the number of programs

    char program[100]; // Buffer for program path

    // Execute programs sequentially
    for (int i = 0; i < n; ++i) {
        fscanf(infile, "%s", program); // Read each program path
        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            return 1;
        }

        if (pid == 0) {
            // Child process: execute the program
            execl(program, program, (char *)NULL);
            // If exec fails
            perror("Exec failed");
            return 1;
        } else {
            // Parent process: wait for the child to finish
            wait(NULL);  // This ensures that the parent waits for the current child before moving to the next program
        }
    }

    // Close the input file
    fclose(infile);
    return 0;
}
