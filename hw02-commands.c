#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <time.h>
#include <signal.h>

#define MAX_STRING_SIZE 1024
#define TIME_LIMIT 5


char** Split(char* string, char* delimiters, int* tokensCount);



int main() {
    char string[MAX_STRING_SIZE];
    int commandCount;
    char **arg = 0;
    scanf("%d\n", &commandCount);
    pid_t pids[commandCount];
    for (int i = 0; i < commandCount; ++i) {
        fgets(string, MAX_STRING_SIZE, stdin);
        int argCount = 0;
        char *delimers = " ";
        arg = Split(string, delimers, &argCount);
        int startTime = atoi(arg[argCount - 1]);
        arg[argCount - 1] = (char *) NULL;
        pids[i] = fork();
        if (pids[i] == 0) {
            sleep(startTime);
            pid_t pid = fork();
            if (pid > 0) {
                sleep(TIME_LIMIT);
                kill(pid, SIGKILL);
                exit(0);
            }
            if (pid == 0)
            {
                execvp(arg[0], arg);
            }
        } else if (pids[i] == -1)
        {
            printf("error of fork\n");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < commandCount; ++i)
        waitpid(pids[i], 0, 0);
    free(arg);
    return 0;
}



char** Split(char* string, char* delimiters, int* wordsCount)
{
    assert(string);
    assert(delimiters);
    assert(wordsCount);
    char** words = 0;
    int counter = 0;
    char* pch = strtok(string, delimiters);
    while (pch != NULL)
    {
        words = (char**)realloc(words, sizeof(char*) * ++counter);
        words[counter - 1] = pch;
        pch = strtok(NULL, delimiters);
    }
    *wordsCount = counter;
    return words;
}
