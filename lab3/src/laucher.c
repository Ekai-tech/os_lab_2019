#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> // Include this header for waitpid 
 
int main(int argc, char *argv[]) { 
    pid_t pid = fork(); // Create a new process 
 
    if (pid < 0) { 
        perror("Fork failed"); 
        return 1; // Exit if fork fails 
    } 
 
    if (pid == 0) { 
        // Child process: execute sequential_min_max 
        char *args[] = {"./sequential_min_max", "42", "10", NULL}; // Example arguments 
        execvp(args[0], args); // Replace child process with sequential_min_max 
        perror("Exec failed"); // If execvp returns, there was an error 
        return 1; 
    } else { 
        // Parent process: wait for child to finish 
        int status; 
        waitpid(pid, &status, 0); // Wait for the specific child process to finish 
        printf("Child process finished with status %d\n", status); 
    } 
 
    return 0; 
}