#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

extern char *__progname;

/* Tokenize the command into arguments */
char **tokenizeCommand(char *command, int *numArgs)
{
    int capacity = 1;
    char **args = malloc(sizeof(char *) * capacity);
    char *token = strtok(command, " ");

    *numArgs = 0;
    while (token != NULL)
    {
        args[*numArgs] = token;
        (*numArgs)++;

        if (*numArgs >= capacity)
        {
            capacity *= 2;
            args = realloc(args, sizeof(char *) * capacity);
        }

        token = strtok(NULL, " ");
    }

    args[*numArgs] = NULL;
    return args;
}

/* Execute the command without arguments */
int executeCommandWithoutArgs(char *command)
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

        /* Return error code */
        return 1;
    }

    /* Child process */
    if (pid == 0)
    {
        /* Execute command */
        if (execlp(command, command, NULL) == -1)
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

/* Execute the command with arguments */
int executeCommandWithArgs(char **args)
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

        /* Free allocated arguments */
        free(args);

        /* Return error code */
        return 1;
    }

    /* Child process */
    if (pid == 0)
    {
        /* Execute command */
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

            /* Free allocated arguments */
            free(args);

            /* Return error code */
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

    /* Free allocated arguments */
    free(args);

    /* Return success code */
    return 0;
}

/* Print error message */
void printMessage(char *program_path)
{

    strcat(program_path, __progname);
}

/* Main function */
int main(void)
{
    /* Declare and initialize variables */
    char *command = NULL;
    size_t size = 0;
    int numArgs,status;
    char **args;
    char *program_path;

    /* Continuous loop */
    while (1)
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

        /* Check if the command has arguments */
        if (strchr(command, ' ') == NULL)
        {
            /* Execute command without arguments */
            status = executeCommandWithoutArgs(command);

            /* Check if execution failed */
            if (status != 0)
            {
                /* Allocate memory for program path */
                program_path = malloc(strlen(__progname) + 3);

                /* Construct program path */
                strcpy(program_path, "./");

                /* Print error message */
                printMessage(program_path);

                /* Free allocated memory */
                free(program_path);
            }
        }
        else
        {
            /* Tokenize command into arguments */
            args = tokenizeCommand(command, &numArgs);

            /* Execute command with arguments */
            status = executeCommandWithArgs(args);

            /* Check if execution failed */
            if (status != 0)
            {
                /* Allocate memory for program path */
                program_path = malloc(strlen(__progname) + 3);

                /* Construct program path */
                strcpy(program_path, "./");
                /* Print error message */
                printMessage(program_path);

                /* Free allocated memory */
                free(program_path);
            }

            /* Free allocated arguments */
            free(args);
        }
    }

    /* Free allocated memory */
    free(command);

    /* Exit program */
    return 0;
}

