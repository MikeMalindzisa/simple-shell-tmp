#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

extern char *__progname;
extern char **environ;

/**
 * main - simple shell program
 *
 * Return: Always 0.
 */
int main(void)
{
    char *command = NULL;
    size_t size = 0;
    char **args;
    pid_t pid;
    char *program_path;
    int i;
    char *path_env = getenv("PATH");
    int found = 0;

    char *path_env_copy;
    char *path_dir;
    
    while (1)
    {
        printf("$ ");
        /* Reset found variable to 0 */
        found = 0;

        /* Read command line from user input */
        if (getline(&command, &size, stdin) == EOF)
        {
            putchar('\n');
            break;
        }

        /* Remove newline character from end of command line */
        command[strlen(command) - 1] = '\0';

        /* Tokenize command line into arguments */
        i = 0;
        args = malloc(sizeof(char *) * (i + 1));
        args[i] = strtok(command, " ");
        while (args[i] != NULL)
        {
            i++;
            args = realloc(args, sizeof(char *) * (i + 1));
            args[i] = strtok(NULL, " ");
        }

        if (strcmp(command, "/bin/ls") == 0)
        {
            found = 1;
        }
        else if (strcmp(args[0], "ls") == 0)
        {
            if (args[0] == NULL)
            {
                break;
            }
            /** Search for ls in PATH
               *make a copy to avoid modifying original */
            path_env_copy = strdup(path_env); 
            path_dir = strtok(path_env_copy, ":");
            while (path_dir != NULL)
            {
                program_path = malloc(strlen(path_dir) + strlen(args[0]) + 2);
                sprintf(program_path, "%s/%s", path_dir, args[0]);

                if (access(program_path, X_OK) == 0)
                {
                    found = 1;
                    args[0] = program_path;
                    break;
                }
                free(program_path);
                path_dir = strtok(NULL, ":");
            }
            free(path_env_copy);
        }

        if (!found)
        {
            /* Print error message if command not found */
          
            fprintf(stderr, "%s: command not found\n", args[0]);
            continue;
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
            program_path = NULL;
            exit(EXIT_FAILURE);

        }
               /* Execute command in child process */
        if (execve(args[0], args, environ) == -1)
        {
            /* Handle execve error */
            /* Add space for "./" and NULL terminator */
            program_path = malloc(strlen("./") + strlen(args[0]) + 1);
            sprintf(program_path, "./%s", args[0]);

            /* Print error message with program name and error message */
            perror(program_path);

            /* Free dynamically allocated memory and exit with failure status */
            free(program_path);
            exit(EXIT_FAILURE);
        }

        /* Free dynamically allocated memory */
        free(program_path);
        free(args);

        /* Exit child process with success status */
        exit(EXIT_SUCCESS);
    }

    /* Wait for child processes to complete */
    while (wait(NULL) > 0);

    /* Free dynamically allocated memory and exit with success status */
    free(command);
    return (0);
}

