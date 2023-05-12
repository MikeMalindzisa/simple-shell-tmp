#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

extern char *__progname;

/**
 * main - simple shell program
 *
 * Return: Always 0.
 */
int main(void)
{
    char *command = NULL;
    size_t size = 0;
    char **args = NULL;
    pid_t pid;
    char *program_path;
    int i, n_args;

    while (1)
    {
        printf("$ ");

        /* Read command line from user input */
        if (getline(&command, &size, stdin) == EOF)
        {
            putchar('\n');
            break;
        }
        fprintf(stderr, "%s\n",command);
        /* Remove newline character from end of command line */
        command[strlen(command) - 1] = '\0';

        /* Tokenize command line into arguments */
       n_args = 0;
       args = realloc(args, 2 * sizeof(char *));
       args[n_args++] = strtok(command, " ");
       while (args[n_args-1] != NULL)
       {
           if (n_args >= 2) {
               args = realloc(args, (n_args + 1) * sizeof(char *));
           }
           args[n_args++] = strtok(NULL, " ");
       }


        /* Check if first argument is "echo" and skip it */
        if (n_args > 1 && strcmp(args[0], "echo") == 0)
        {
            free(args[0]);
            n_args--;
            for (i = 0; i < n_args; i++)
            {
                args[i] = args[i+1];
            }
        }


        /* Create child process to execute command */
        pid = fork();
        if (pid < 0)
        {
            /* Handle fork error
             * Add space for "./" and NULL terminator
             */
            program_path = malloc(strlen(__progname) + 3);
            strcpy(program_path, "./");
            strcat(program_path, __progname);

            /* Print error message with program name and error message */
            fprintf(stderr, "%s: ", program_path);
            perror("");

            /* Free dynamically allocated memory and exit with failure status */
            free(program_path);
            exit(EXIT_FAILURE);
        }
        if (pid == 0)
        {
            /* Execute command in child process */
            if (execve(args[0], args, NULL) == -1)
            {
                /* Handle execve error */
                /* Add space for "./" and NULL terminator */
                program_path = malloc(strlen(__progname) + 3);
                strcpy(program_path, "./");
                strcat(program_path, __progname);

                /* Print error message with program name and error message */
                fprintf(stderr, "%s: ", program_path);
                perror("");

                /* Free dynamically allocated memory and exit with failure status */
                free(program_path);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            /* Wait for child process to complete */
            wait(NULL);
        }

        /* Free dynamically allocated memory for arguments */
        for (i = 0; i < n_args + 1; i++)
        {
            if (args[i] != NULL)
            {
                free(args[i]);
            }
        }
        free(args);

        args = NULL;
    }

    /* Free dynamically allocated memory and exit with success status */
    free(command);
    return (0);
}
