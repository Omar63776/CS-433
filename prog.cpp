#include <stdio.h>      // Standard input-output library for printing and taking input
#include <unistd.h>     // Provides access to the POSIX operating system API (for fork(), execvp(), etc.)
#include <sys/types.h>  // Defines data types used in system calls (for pid_t)
#include <sys/wait.h>   // Contains macros related to wait() (used for process synchronization)
#include <string.h>     // String handling functions (e.g., strcpy, strcmp, strtok)
#include <stdlib.h>     // General utilities (e.g., exit())
#include <fcntl.h>      // File control options (used for file manipulation)

#define MAX_LINE 80     // Defines the maximum length of the command line input

int main(void) {
    char *args[MAX_LINE/2 + 1]; // Array of strings to store command arguments, split by spaces
    int should_run = 1;         // Flag to determine if the shell should continue running
    char history[MAX_LINE];     // Buffer to store the most recent command entered
    
    while (should_run) {
        printf("osh> ");        // Display the shell prompt
        fflush(stdout);         // Ensure the prompt is displayed immediately

        char input[MAX_LINE];   // Buffer to store user input
        fgets(input, MAX_LINE, stdin);  // Get the input from the user
        
        // Check if the user entered the history command "!!"
        if (strcmp(input, "!!\n") == 0) {
            if (strlen(history) == 0) { // If no history is available
                printf("No commands in history\n");
                continue;       // Continue to the next iteration of the loop
            } else {
                strcpy(input, history); // Copy the last command from history to input
                printf("%s\n", history); // Echo the history command to the user
            }
        } else {
            strcpy(history, input); // Save the current command to history
        }

        // Tokenize the input by splitting the string based on spaces and newlines
        char *token = strtok(input, " \n");
        int i = 0;                 // Initialize argument index
        int background = 0;        // Flag to check if the command should run in the background
        while (token != NULL) {    // Loop through all the tokens (arguments)
            args[i++] = token;     // Store each token in the args array
            token = strtok(NULL, " \n");  // Get the next token
        }
        args[i] = NULL;            // Null-terminate the argument array

        // Check if the last argument is "&", indicating a background process
        if (i > 0 && strcmp(args[i - 1], "&") == 0) {
            background = 1;        // Set background flag to true
            args[i - 1] = NULL;    // Remove "&" from the argument list
        }

        // Check if the user entered the "exit" command
        if (strcmp(args[0], "exit") == 0) {
            should_run = 0;        // Set the flag to stop the shell loop
            continue;              // Exit the current iteration
        }

        pid_t pid = fork();        // Create a new process (fork)
        if (pid < 0) {             // Error occurred during fork
            perror("Fork failed"); // Print error message
            exit(1);               // Exit with an error status
        } else if (pid == 0) {     // Child process
            execvp(args[0], args); // Replace the current process with the given command
            perror("exec failed"); // If execvp fails, print an error message
            exit(1);               // Exit the child process with error status
        } else {                   // Parent process
            if (!background) {     // If the command is not to run in the background
                wait(NULL);        // Parent waits for the child process to finish
            }
        }
    }

    return 0;  // Return 0 to indicate the program executed successfully
}
