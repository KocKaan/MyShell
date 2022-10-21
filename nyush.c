#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

#include <libgen.h>
//#include <limits.h>

#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

int fd;
static char **args;
char *inputBuff;
char *cleanData[512];
static char prompt[512];
char cwd[1024];
pid_t pid;
int outputRedirectionFlag;
int inputRedirectionFlag;
// int outputAppendFlag;
char *inputFile;
char *outputFile;

void redirectInput(char *);
void myPrompt();
void redirectOutput(char *);
void clean();
char *skipWhite(char *);
static int runCommand(int, int, int, char *);
void cleanSpace(char *);
// void redirectAppendOutput(char *);
static int inbuilt(char *, int, int, int);
void runInput();
void myCd();

void clean()
{
    cwd[0] = '\0';
    prompt[0] = '\0';
    outputRedirectionFlag = 0;
    inputRedirectionFlag = 0;
    pid = 0;
    fd = 0;
}

void myPrompt()
{
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        strcpy(prompt, "[nyush ");
        char *base;
        base = basename(cwd);
        strcat(prompt, base);
        strcat(prompt, "]$ ");
    }
    return;
}

char *skipWhite(char *str)
{
    int i = 0;
    int j = 0;
    char *temp;
    if (NULL == (temp = (char *)malloc(sizeof(str) * sizeof(char))))
    {
        perror("Eror with malloc: ");
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

    if (args[1] == NULL || args[2] != NULL)
    {
        fprintf(stderr, "Error: invalid command");
    }

    else if (chdir(args[1]) < 0)
    {
        fprintf(stderr, "Error: invalid directory");
    }
}
/*
void redirectAppendOutput(char *cleanData)
{
    char *val[128];
    char *newCleanData;
    newCleanData = strdup(cleanData);

    val[0] = strtok(newCleanData, ">>\n");

    int i = 1;
    while ((val[i] = strtok(NULL, ">>\n")) != NULL)
    {
        i++;
    }

    char *s1;
    s1 = strdup(val[1]);
    outputFile = skipWhite(s1);

    printf("KAOFJAEJGJEJ");
    cleanSpace(val[0]);
    return;
}
*/

void redirectInput(char *cleanData)
{
    char *val[128];
    char *newCleanData;
    newCleanData = strdup(cleanData);

    val[0] = strtok(newCleanData, "<\n");

    int i = 1;
    while ((val[i] = strtok(NULL, "<\n")) != NULL)
    {
        i++;
    }

    // printf("%s", val[1]);
    char *skip;
    skip = strdup(val[1]);
    inputFile = skipWhite(skip);

    cleanSpace(val[0]);
    return;
}

void redirectOutput(char *cleanData)
{
    char *val[128];
    char *newCleanData;
    newCleanData = strdup(cleanData);

    val[0] = strtok(newCleanData, ">\n");

    int i = 1;
    while ((val[i] = strtok(NULL, ">\n")) != NULL)
    {
        i++;
    }

    char *skip;
    skip = strdup(val[1]);
    outputFile = skipWhite(skip);

    cleanSpace(val[0]);
    return;
}

void cleanSpace(char *str)
{
    args = malloc(10000 * sizeof(char *));

    if (args == NULL)
    {
        perror("malloc call unsuccessful");
        exit(1);
    }

    char *token;
    int element = 0;

    token = strtok(str, " \n");

    while (token != NULL)
    {

        args[element] = malloc(10000 * sizeof(char));
        strcpy(args[element], token);

        element++;
        token = strtok(NULL, " \n");
    }
    args[element] = NULL;
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
        if (last == 0 && input == 0 && first == 1)
        {

            dup2(myFileDescriptor[1], 1);
        }
        else if (last == 0 && first == 0 && input != 0)
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
        /*
        else if (strpbrk(cleanData, ">>"))
        {
            outputAppendFlag = 1;
            redirectAppendOutput(cleanData);
        }
        */
        if (outputRedirectionFlag)
        {
            if ((outputFileDescriptor = creat(outputFile, 0644)) < 0)
            {
                fprintf(stderr, "Error: invalid file\n");
                return (EXIT_FAILURE);
            }
            dup2(outputFileDescriptor, 1);
            close(outputFileDescriptor);
            outputRedirectionFlag = 0;
        }

        /*
        if (outputAppendFlag)
        {
            if ((outputFileDescriptor = open(outputFile, O_APPEND)) < 0)
            {
                fprintf(stderr, "Failed to open %s for writing\n", outputFile);
                return (EXIT_FAILURE);
            }
            dup2(outputFileDescriptor, 1);
            close(outputFileDescriptor);
            outputAppendFlag = 0;
        }
        */

        if (inputRedirectionFlag)
        {

            if ((inputFileDescriptor = open(inputFile, O_RDONLY, 0)) < 0)
            {
                fprintf(stderr, "Error: invalid file\n");
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
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);

    if (args[0] != NULL)
    {
        if (!(strcmp(args[0], "exit")))
        {
            if (args[1] != NULL)
            {
                fprintf(stderr, "Error: invalid command\n");
                return 1;
            }
            else
            {
                exit(0);
            }
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

    cleanData[0] = strtok(inputBuff, "|\n");

    int commandCounter = 1;
    while ((cleanData[commandCounter] = strtok(NULL, "|\n")) != NULL)
    {
        commandCounter++;
    }

    cleanData[commandCounter] = NULL;

    int i = 0;
    for (i = 0; i < commandCounter - 1; i++)
    {
        input = inbuilt(cleanData[i], input, first, 0);
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
        signal(SIGINT, SIG_IGN);
        signal(SIGQUIT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);

        char buffer[512] = {};
        inputBuff = buffer;
        size_t bufsize = 512;

        printf("%s", prompt);
        fflush(stdout);

        getline(&inputBuff, &bufsize, stdin);

        // printf("%s", inputBuff);
        //  For the case of empty space
        if (!(strcmp(inputBuff, "\n") && strcmp(inputBuff, "")))
        {
            continue;
        }

        runInput();

    } while (counter);

    return 0;
}
