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

void SetAround(int** field, int n)
{
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if (field[i][j] == 10)
                for (int k = i - 1; k < i + 2; ++k)
                    for (int l = j - 1; l < j + 2; ++l)
                    {
                        if ((k >= n) || (l >= n) || (k < 0) || (l < 0))
                            continue;
                        if (field[k][l] != 10)
                            field[k][l] += 1;
                    }
}


int** GenerateField(int n)
{
    srand(time(NULL));
    int **res_field = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
    {
        res_field[i] = (int *)malloc(n * sizeof(int));
        for (int j = 0; j < n; j++)
        {
            int random = rand() % 101;
            res_field[i][j] = (random < MINE_PERCENT) ? 10 : 0;
        }
    }
    SetAround(res_field, n);
    return res_field;
}

void PrintField(int** field, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
            printf("%2d ", field[i][j]);
        printf("\n");
    }
}

void ClearField(int** field, int n)
{
    for (int i = 0; i < n; i++)
        free(field[i]);
    free(field);
}



int main()
{
    int** field = GenerateField(FIELD_SIZE);
    PrintField(field, FIELD_SIZE);
    ClearField(field, FIELD_SIZE);
    return 0;
}