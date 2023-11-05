#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

char **argument_list;
char *builtin_functions_name_list[] = {"help", "-", "cd", "about"};

void exit_shell()
{
    printf("Bye Bye");
    exit(0);
}

void about()
{
    system("cat banner.txt");
}

void cd(char **cmd)
{
    if (cmd[1] == NULL)
    {
        printf("Wrong Usage! Use cd /directory");
        return;
    }
    else
    {
        if (chdir(cmd[1]) != 0)
        {
            printf("Directory not found!");
            return;
        }
    }
}

void help()
{
    printf("Uasge (a few built-in commands:\n\n");
    printf(" help -> for list of built-in commands\n");
    printf(" - (a dash) -> exit\n");
    printf(" about -> about the program\n");
}

void init()
{
    printf("\033[1;32m");
    printf("\n");
    system("cat banner.txt");
    printf("\n");
    system(" echo HELLO $USER");
}

int check_builtins(char *command) 
{
    int builtin_number = -1;
    int size = sizeof(builtin_functions_name_list) / sizeof(char *);
    for (int i = 0; i < size; i++)
    {
        if (strcmp(command, builtin_functions_name_list[i]) == 0)
        {
            builtin_number = i;
            break;
        }
    }
    switch (builtin_number)
    {
    case 0:
        help();
        return 1;
    case 1:
        exit_shell();
        return 1;
    case 2:
        cd(argument_list);
        return 1;
    case 3:
        about();
        return 1;
    default:
        return 0;
    }
}

#define DELIMITER " \t\r\n\a"

char **parse(char *input)
{
    int position = 0;
    char **arguments = malloc(sizeof(char *) * 1000);
    char *inputed;

    inputed = strtok(input, DELIMITER);
    while (inputed != NULL)
    {
        arguments[position] = inputed;
        // printf("%s",arguments[position]);
        position++;
        inputed = strtok(NULL, DELIMITER);
    }
    arguments[position] = NULL;
    return arguments;
}

int execute()
{
    char *command;
    while (1)
    {
        command = readline("🐚 -> ");
        if (strcmp(command, "") == 0) {
            continue;
        }
        add_history(command);
        argument_list = parse(command);
        if (check_builtins(command))
        {
            continue;
        }
        int pid = fork();
        if (pid == 0)
        {
            int status_code = execvp(command, argument_list);
            printf("\033[5;31m ERROR! Command not found.\n");
            if (status_code == -1)
            {
                printf("\033[25;32m\n");
                return 1;
            }
        }
        else
        {
            int status;
            do
            {
                int wpid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
        free(command);
    }
    return 0;
}

int main()
{
    init();
    execute();
    return 0;
}
