#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>

extern char *__progname;

/* Execute the /bin/ls command without arguments */
int executeCommand()
{
    char *program_path;
    int status;

    /* Fork a child process */
    pid_t pid = fork();

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

        return 1;
    }

    /* Child process */
    if (pid == 0)
    {
        /* Create arguments array */
        char *args[] = { "/bin/ls", NULL };

        /* Execute /bin/ls command */
        if (execve("/bin/ls", args, NULL) == -1)
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

            return 1;
        }
    }
    /* Parent process */
    else
    {
        /* Wait for child process to complete */
        wait(&status);

        /* Check if child process exited normally */
        if (WIFEXITED(status))
        {
            /* Return exit status of child process */
            return WEXITSTATUS(status);
        }
    }

    /* Return success code */
    return 0;
}

/* Main function */
int main(void)
{
    /* Declare variables */
    char *command = NULL;
    size_t bufsize = 0;
    ssize_t input_length;
    int status;

    /* Check if running with a terminal */
    bool interactive = isatty(STDIN_FILENO);

    while (true)
    {
        /* Read user input */
        if (interactive)
            printf("$ ");

        /* Read user input using getline */
        input_length = getline(&command, &bufsize, stdin);

        /* Check if getline encountered an error or reached EOF */
        if (input_length == -1)
            break;

        /* Remove trailing newline character */
        if (command[input_length - 1] == '\n')
            command[input_length - 1] = '\0';

        /* Check if the command is /bin/ls */
        if (strcmp(command, "/bin/ls") == 0)
        {
            /* Execute /bin/ls command */
            status = executeCommand();
        }
        else
        {
            /* Print error message */
            fprintf(stderr, "%s: No such file or directory\n", __progname);
            continue;
        }

        /* Check if execution failed */
        if (status != 0)
        {
            /* Print error message */
            fprintf(stderr, "%s: Command execution failed\n", __progname);
        }
    }

    /* Free allocated memory for command */
    free(command);

    /* Exit program */
    return 0;
}

