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
char *cleanData[512];

// declare functions
void createPrompt();
void runInput();
static int inbuilt(char *, int, int, int);

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
static int inbuilt(char *cleanData, int input, int first, int last)
{
    char *newCleanData;
    newCleanData = strdup(cleanData);

    if (args[0] != NULL)
    {
        if (!strcmp("cd", args[0]))
        {
            s_cd();
            return 1;
        }
    }
    printf("lol");
}

void runInput()
{

    cleanData[0] = strtok(inputBuff, "|");

    int commandCounter = 1;
    while ((cleanData[commandCounter] = strtok(NULL, "|")) != NULL)
    {
        commandCounter++;
    }
    cleanData[commandCounter] = NULL;

    int input = 0;
    int first = 1;
    for (int i = 0; i < commandCounter - 1; i++)
    {
        input = inbuilt(cleanData[i], input, first, 1);
        first = 0;
    }
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
        printf("%s\n", inputBuff);
        runInput();

    } while (counter);
}