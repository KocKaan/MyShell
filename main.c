#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <unistd.h>

// declare variable
static char *args[512];
static char prompt[512];
char *inputBuff;
char cwd[1024];

// declare functions
void createPrompt();

void reset()
{
    cwd[0] = '\0';
    prompt[0] = '0';
}

void createPrompt()
{
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        strcpy(prompt, "Project shell> ");
        strcat(prompt, cwd);
        strcat(prompt, ":$ ");
    }
    else
    {

        perror("Error in getting curent working directory: ");
    }
    return;
}

int main()
{
    int counter = 1;

    do
    {
        reset();
        createPrompt();
        inputBuff = readline(prompt);

        // For the case of empty space
        if (!(strcmp(inputBuff, "\n") && strcmp(inputBuff, "")))
        {
            continue;
        }

        // compares first 4 chars of input
        if (!(strncmp(inputBuff, "exit", 4) && strncmp(inputBuff, "quit", 4)))
        {
            counter = 0;
            break;
        }
        printf("%s", inputBuff);

    } while (counter);
}