#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#include <dirent.h>

int fd;
static char *args[512];
static char prompt[512];
char *inputBuff;
char *cleanData[512];
char cwd[1024];
pid_t pid;
int outputRedirectionFlag;
int inputRedirectionFlag;
char *inputFile;
char *outputFile;

void clean();
void myCd();
char *skipwhite(char *);
void cleanSpace(char *);
void redirectInput(char *);
void redirectOutput(char *);
static int inbuilt(char *, int, int, int);
void runInput();
static int runCommand(int, int, int, char *);
void myPrompt();

void clean()
{
    fd = 0;
    outputRedirectionFlag = 0;
    inputRedirectionFlag = 0;
    cwd[0] = '\0';
    prompt[0] = '\0';
    pid = 0;
}

void myPrompt()
{
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        strcpy(prompt, "Proiect shell> ");
        strcat(prompt, cwd);
        strcat(prompt, ":$ ");
    }
    else
    {

        perror("Error in getting curent working directory: ");
    }
    return;
}

char *skipwhite(char *str)
{
    int i = 0;
    int j = 0;
    char *temp;
    if (NULL == (temp = (char *)malloc(sizeof(str) * sizeof(char))))
    {
        perror("Memory Error: ");
        return NULL;
    }

    while (str[i++])
    {
        if (str[i - 1] != ' ')
        {
            temp[j++] = str[i - 1];
        }
    }
    temp[j] = '\0';
    return temp;
}

void myCd()
{
    char *home_dir = "/home";
    if ((args[1] == NULL) || (!(strcmp(args[1], "~") && strcmp(args[1], "~/"))))
        chdir(home_dir);
    else if (chdir(args[1]) < 0)
        perror("No such file or directory: ");
}

void redirectInput(char *cleanData)
{
    char *val[128];
    char *newCleanData, *s1;
    newCleanData = strdup(cleanData);

    int m = 1;
    val[0] = strtok(newCleanData, "<");
    while ((val[m] = strtok(NULL, "<")) != NULL)
        m++;

    s1 = strdup(val[1]);
    inputFile = skipwhite(s1);

    cleanSpace(val[0]);
    return;
}

void redirectOutput(char *cleanData)
{
    char *val[128];
    char *newCleanData, *s1;
    newCleanData = strdup(cleanData);

    val[0] = strtok(newCleanData, ">");

    int i = 1;
    while ((val[i] = strtok(NULL, ">")) != NULL)
    {
        i++;
    }

    s1 = strdup(val[1]);
    outputFile = skipwhite(s1);

    cleanSpace(val[0]);
    return;
}

void cleanSpace(char *str)
{

    args[0] = strtok(str, " ");

    int i = 1;
    while ((args[i] = strtok(NULL, " ")) != NULL)
    {
        i++;
    }

    args[i] = NULL;
}

static int runCommand(int input, int first, int last, char *cleanData)
{
    int returns;
    int myFileDescriptor[2];
    int inputFileDescriptor, outputFileDescriptor;

    if (-1 == (returns = pipe(myFileDescriptor)))
    {
        perror("pipe error: ");
        return 1;
    }

    pid = fork();
    // create child process that is copy of parent process
    //  0> process id of the child process to the parent,  0== for child process
    if (pid == 0)
    {
        // stdin fd represented by 0, stdout fl reprensted by 1
        // dup2 used to create copy of an existing file descriptor
        if (first == 1 && last == 0 && input == 0)
        {

            dup2(myFileDescriptor[1], 1);
        }
        else if (first == 0 && last == 0 && input != 0)
        {
            dup2(input, 0);
            dup2(myFileDescriptor[1], 1);
        }
        else
        {
            dup2(input, 0);
        }

        if (strchr(cleanData, '<'))
        {
            inputRedirectionFlag = 1;
            redirectInput(cleanData);
        }
        else if (strchr(cleanData, '>'))
        {
            outputRedirectionFlag = 1;
            redirectOutput(cleanData);
        }

        if (outputRedirectionFlag)
        {
            if ((outputFileDescriptor = creat(outputFile, 0644)) < 0)
            {
                fprintf(stderr, "Failed to open %s for writing\n", outputFile);
                return (EXIT_FAILURE);
            }
            dup2(outputFileDescriptor, 1);
            close(outputFileDescriptor);
            outputRedirectionFlag = 0;
        }

        if (inputRedirectionFlag)
        {

            if ((inputFileDescriptor = open(inputFile, O_RDONLY, 0)) < 0)
            {
                fprintf(stderr, "Failed to open %s for reading\n", inputFile);
                return (EXIT_FAILURE);
            }
            dup2(inputFileDescriptor, 0);
            close(inputFileDescriptor);
            inputRedirectionFlag = 0;
        }

        if (execvp(args[0], args) < 0)
        {
            fprintf(stderr, "%s: Command not found\n", args[0]);
        }
        exit(0);
    }

    else
    {
        waitpid(pid, 0, 0);
    }

    if (last == 1)
    {
        close(myFileDescriptor[0]);
    }

    if (input != 0)
    {
        close(input);
    }

    close(myFileDescriptor[1]);
    return (myFileDescriptor[0]);
}

static int inbuilt(char *cleanData, int input, int isfirst, int islast)
{
    char *newCleanData;
    newCleanData = strdup(cleanData);
    cleanSpace(cleanData);

    if (args[0] != NULL)
    {
        if (!(strcmp(args[0], "exit")))
        {
            exit(0);
        }
        if (!strcmp("cd", args[0]))
        {
            myCd();
            return 1;
        }
    }
    return (runCommand(input, isfirst, islast, newCleanData));
}

void runInput()
{
    int input = 0;
    int first = 1;

    cleanData[0] = strtok(inputBuff, "|");

    int commandCounter = 1;
    while ((cleanData[commandCounter] = strtok(NULL, "|")) != NULL)
    {
        commandCounter++;
    }

    cleanData[commandCounter] = NULL;

    int i = 0;
    for (i = 0; i < commandCounter - 1; i++)
    {
        input = inbult(cleanData[i], input, first, 0);
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
        clean();
        myPrompt();
        inputBuff = readline(prompt);

        // For the case of empty space
        if (!(strcmp(inputBuff, "\n") && strcmp(inputBuff, "")))
        {
            continue;
        }

        // compares first 4 chars of input
        if (!(strncmp(inputBuff, "exit", 4)))
        {
            counter = 0;
            break;
        }

        runInput();

    } while (counter);

    if (counter == 0)
    {
        printf("\nSe inchide shellul.\n");
        exit(0);
    }

    return 0;
}
