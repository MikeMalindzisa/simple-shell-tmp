#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

extern char *__progname;

/* Main function */
int main(void)
{
    /* Declare and initialize variables */
    char *command = NULL;
    size_t size = 0;
    char *args[] = {NULL};
    pid_t pid;
    char *program_path;

    /* Continuous loop */
    while(1)
    {
        /* Print prompt */
        printf("$ ");

        /* Read user input */
        if (getline(&command, &size, stdin) == EOF)
        {
            putchar('\n');
            break;
        }

        /* Remove newline character from input */
        command[strlen(command) - 1] = '\0';

        /* Fork a child process */
        pid = fork();

        /* Check for forking error */
        if (pid < 0)
        {
            /* Allocate memory for program path */
            program_path = malloc(strlen(__progname) + 3);

            /* Construct program path */
            strcpy(program_path, "./");
            strcat(program_path, __progname);

            /* Print error message */
            fprintf(stderr, "%s: ", program_path);
            perror("");

            /* Free allocated memory */
            free(program_path);

            /* Return error code */
            return (1);
        }

        /* Child process */
        if (pid == 0)
        {
            /* Execute command */
            if (execve(command, args, NULL) == EOF)
            {
                /* Allocate memory for program path */
                program_path = malloc(strlen(__progname) + 3);

                /* Construct program path */
                strcpy(program_path, "./");
                strcat(program_path, __progname);

                /* Print error message */
                fprintf(stderr, "%s: ", program_path);
                perror("");

                /* Free allocated memory */
                free(program_path);

                /* Return error code */
                return (1);
            }
        }
        /* Parent process */
        else
        {
            /* Wait for child process to complete */
            wait(NULL);
        }
    }

    /* Free allocated memory */
    free(command);

    /* Exit program */
    return (0);
}

