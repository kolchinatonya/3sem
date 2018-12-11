#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define FIELD_SIZE 9
#define MINE_PERCENT 25

typedef struct Position
{
    int x;
    int y;
}Position;

char** GenerateEmptyField(int n)
{
    srand(time(NULL));
    char **res_field = (char **)malloc(n * sizeof(char *));
    for (int i = 0; i < n; i++)
    {
        res_field[i] = (char *)malloc(n * sizeof(int));
        for (int j = 0; j < n; j++)
            res_field[i][j] = '*';
    }
    return res_field;
}

void PrintFieldWhithIndexes(char** field, int n)
{
    printf(" %2c ", ' ');
    for (int i = 1; i < n + 1; ++i)
        printf("%2d ", i);
    printf("\n");
    printf("%2c ", ' ');
    for (int i = 0; i < 3 * n; ++i)
        printf("%c", '-');
    printf("\n");
    for (int i = 0; i < n; i++)
    {
        printf("%2d%c ", i + 1,'|');
        for (int j = 0; j < n; j++)
            printf("%2c ", field[i][j]);
        printf("\n");
    }
    printf("\n");
}

void ClearField(char** field, int n)
{
    for (int i = 0; i < n; i++)
        free(field[i]);
    free(field);
}

int main()
{
    char** field = GenerateEmptyField(FIELD_SIZE);
    int flag = 1;
    while (flag)
    {
        system ("clear");
        PrintFieldWhithIndexes(field, FIELD_SIZE);
        printf("Put your command: (""F X Y"" or ""O X Y"")\n");
        char command;
        Position cur;
        scanf("%c %d %d", &command, &cur.x, &cur.y);
        cur.x--;
        cur.y--;
        if (command == 'F')
            field[cur.x][cur.y] = '%';
        if (command == 'O')
        {
            field[cur.x][cur.y] = '0';
            system ("clear");
            flag = 0;
            PrintFieldWhithIndexes(field, FIELD_SIZE);
        }
    }
    ClearField(field, FIELD_SIZE);
    return 0;
}