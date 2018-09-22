#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

char** Split(char* string, char* delimiters, int* tokensCount);

int main() {
    // FIXIT: вместо магический чисел 1024 и 20 заведите константы #define MAX_STRING_SIZE 1024 и т.п.
    char string[1024], delimiters[20];
    int wordsCount = 0;
    printf("Input string:\n");
    fgets(string, 1024, stdin);
    printf("Input delimiters:\n");
    fgets(delimiters, 20, stdin);
    char** words = Split(string, delimiters, &wordsCount);
    for (int i = 0; i < wordsCount; ++i)
    // words[i] равносильно *(words + i)
        printf("%s\n", *(words + i));
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
        // FIXIT: непонятно, почему мы выделяем в два раза больше памяти, чем у нас слов на текущий момент?
        words = (char**)realloc(words, sizeof(char*) * ++counter * 2);
        words[counter - 1] = pch;
        pch = strtok(NULL, delimiters);
    }
    *wordsCount = counter;
    return words;
}
