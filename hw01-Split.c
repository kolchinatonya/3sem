#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_STRING_SIZE 1024


char** Split(char* string, char* delimiters, int* tokensCount);

int main() {
    char string[MAX_STRING_SIZE], delimiters[MAX_STRING_SIZE];
    int wordsCount = 0;
    printf("Input string:\n");
    fgets(string, 1024, stdin);
    printf("Input delimiters:\n");
    fgets(delimiters, 20, stdin);
    char** words = Split(string, delimiters, &wordsCount);
    for (int i = 0; i < wordsCount; ++i)
        printf("%s\n", words[i]);
    free(words);
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
