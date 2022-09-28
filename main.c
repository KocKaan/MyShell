#include <stdio.h>

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
        inputBuff = readLine(prompt);

        if (!strncmp(inputBuff, "exit"))

    } while (counter);

    printf("kaan\n");
}