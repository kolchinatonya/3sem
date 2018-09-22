#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Count(i) - длина строки на данной итерации
int Count(int n)
{
    return (int)pow(2, n) - 1;
}

void GenerateString(int n, char* str)
{
    if (n < 1)
        return;
    str[0] = 'a';
    for (int i = 1; i < n; ++i) {
        char next_letter = (char)('a' + i);
        str[Count(i)] = next_letter;
        strncpy(str + Count(i) + 1, str, Count(i));
    }
    str[Count(n)] = '\0';
}

int main() {
    int n, count;
    scanf("%d", &n);
    while (n != 0) {
        count = Count(n);
        char *str;
        str = (char *) malloc(count * sizeof(char));
        GenerateString(n, str);
        printf("%s\n", str);
        free(str);
        scanf("%d", &n);
    }
    return 0;
}
