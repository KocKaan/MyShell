#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <unistd.h>

// declare variable
static char *newSpaceData[512];
static char prompt[512];
char *inputBuff;
char cwd[1024];
char *cleanData[512];
pid_t pid;

// declare functions
void createPrompt();
void runInput();
static int inbuilt(char *, int, int, int);
void myCd();
static int runCommand(int, int, int, char *);

/*
1) check the cd and make sure you have realtive and absolute path
*/

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
void myCd()
{
    char *home = "/home";
    if ((newSpaceData[1] == NULL) || (!(strcmp(newSpaceData[1], "~") && strcmp(newSpaceData[1], "~/"))))
    {
        printf("running the cd ");
        chdir(home);
    }
    else if (chdir(newSpaceData[1]) < 0)
    {
        perror("No such file or directory: ");
    }
    printf("%s", newSpaceData[1]);
}

void cleanSpace(char *line)
{

    newSpaceData[0] = strtok(line, " ");
    int i = 1;
    while ((newSpaceData[i] = strtok(NULL, " ")) != NULL)
    {
        i++;
    }
    newSpaceData[i] = NULL;
}
static int runCommand(int input, int first, int last, char *cmd_exec)
{
    int returns;
    // fd[0] read end fd[1] write enf of pipe
    int myFileDescriptor[2];

    // check if pipe works
    if (-1 == (returns = pipe(myFileDescriptor)))
    {
        printf("Pipe error occured");
        return 1;
    }

    // create child process that is copy of parent process
    //  0> process id of the child process to the parent,  0== for child process
    pid = fork();

    if (pid == 0)
    {
        printf("kaan");
    }
    else
    {
        // we are in parent process suspending it
        //  wait for any child process whose process group ID is equal to that of calling process
        waitpid(pid, 0, 0);
    }

    // stdin fd represented by 0, stdout fl reprensted by 1
    // dup2 used to create copy of an existing file descriptor
    if (input == 0 && first == 1 && last == 0)
    {
        dup2(myFileDescriptor[1], 1);
    }
    else if (input != 0 && first == 0 && last == 0)
    {
        // copied input to standard input file descriptor
        dup2(input, 0);
        // put into myFileDescriptor
        dup2(myFileDescriptor[1], 1);
    }
    else
    {
        dup2(input, 0);
    }
}

static int inbuilt(char *cleanData, int input, int first, int last)
{
    char *newCleanData;

    // this copies the previos line seperated by |
    newCleanData = strdup(cleanData);

    // this seperate line based on white space
    cleanSpace(cleanData);

    if (newSpaceData[0] != NULL)
    {
        if (strcmp("cd", newSpaceData[0]) == 0)
        {
            printf("In the CD now");
            myCd();
            return 1;
        }
    }
    printf("lol\n");
    return runCommand(input, first, last, newCleanData);
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
    int i = 0;
    for (i = 0; i < commandCounter - 1; i++)
    {
        printf("in the runInput");
        input = inbuilt(cleanData[i], input, first, 1);
        first = 0;
    }
    input = inbuilt(cleanData[i], input, first, 1);
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
        printf("%s\n", inputBuff);

        runInput();

    } while (counter);
}