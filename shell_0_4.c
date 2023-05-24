#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>

extern char *__progname;

/* Execute a command with arguments */
int executeCommand(char *args[])
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
        /* Execute command with arguments */
        if (execvp(args[0], args) == -1)
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
    char *token;
    char *args[256];
    int arg_index = 0;

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

        /* Tokenize the command and arguments */

        token = strtok(command, " ");

        /* Extract the command and arguments */
        while (token != NULL && arg_index < 255)
        {
            args[arg_index++] = token;
            token = strtok(NULL, " ");
        }
        args[arg_index] = NULL;

        /* Check if the command is ls or /bin/ls */
        if (strcmp(args[0], "ls") == 0)
        {
            args[0] = "/bin/ls";
        }
        else if (strcmp(args[0], "exit") == 0)
        {
            /* Free allocated memory for command */
            free(command);

            /* Exit program */
            return 0;
        }

        /* Check if the command exists in the PATH */
        if (access(args[0], F_OK) == 0)
        {
            /* Execute command with arguments */
            status = executeCommand(args);
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

        /* Reset argument index */
        arg_index = 0;
    }

    /* Free allocated memory for command */
    free(command);

    /* Exit program */
    return 0;
}

