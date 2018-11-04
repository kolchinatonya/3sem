//при запуске программы с утилитой time получено ускорение ~ 2,27 
//({k = 1000; n = 1000} / {k = 1; n = 1000})

// Число нитей можно было варьировать в интервале от 1 до числа процессоров на компьютере (видимо 4 в вашем случае)
// При числе нитей больше числа процессоров результат будет тот же, что и для 4 или, может, чуть хуже.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int **GenerateZeroMatrix(int n)
{
    int **A = (int **) malloc(sizeof(int *) * n);
    for (int i = 0; i < n; i++)
    {
        int *a = (int *) malloc(sizeof(int) * n);
        for (int j = 0; j < n; j++)
            a[j] = 0;
        A[i] = a;
    }
    return A;
}

int **GenerateRandomMatrix(int n)
{
    int **A = GenerateZeroMatrix(n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            A[i][j] = rand() % 100;
    return A;
}

void PrintMatrix(int **A, int n, char *name)
{
    printf("Matrix %s:\n", name);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
            printf("%5d ", A[i][j]);
        printf("\n");
    }
    printf("\n");
}

int** MultiplyMatrices(int** A, int** B, int n)
{
    int** C = GenerateZeroMatrix(n);
    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j++)
            for(int k = 0; k < n; k++)
                C[i][j] += A[i][k] * B[k][j];
    return C;
}

typedef struct
{
    //A[n,n]; B[n,n]; k - колчичество потоков
    int **A, **B, **C, n, k;
}ParamsForThreadMultiply;

ParamsForThreadMultiply PARAMS;

void *ThreadMultiplyMatrices(void *thread_ind)
{
    int strings_for_thread = PARAMS.n / PARAMS.k;
    int ind_to_write = (int) thread_ind * strings_for_thread;
    for (int i = 0; i < strings_for_thread; i++)
    {
        for (int j = 0; j < PARAMS.n; j++)
            for (int k = 0; k < PARAMS.n; k++)
                PARAMS.C[i + ind_to_write][j] += PARAMS.A[i + ind_to_write][k] * PARAMS.B[k][j];
    }
    return NULL;
}

void DeleteMatrix(int **A, int n)
{
    for (int i = 0; i < n; i++)
        free(A[i]);
    free(A);
}

// argv[1] - количество потоков, argv[2] - размер матрицы
int main(int argc, char ** argv)
{

    long k = 0, n = 0;
    k = strtol(argv[1], NULL, 10);
    n = strtol(argv[2], NULL, 10);

    if (n % k != 0)
    {
        printf("ERROR! (%ld %% %ld != 0) !", n, k);
        exit(1);
    }

    int **A = GenerateRandomMatrix(n);
    int **B = GenerateRandomMatrix(n);
    int **C = GenerateZeroMatrix(n);

    pthread_t thread[k];

    ParamsForThreadMultiply params = {A, B, C, n, k};

    // Можно было структуру params передать в качестве параметра в ThreadMultiplyMatrices, и глобальную переменную не использовать.
    PARAMS = params;

    for (int i = 0; i < k; i++)
    {
        int result = pthread_create(&(thread[i]), (pthread_attr_t *) NULL,
                                ThreadMultiplyMatrices, (void *) i);

        if (result != 0)
        {
            printf("Error on thread create, return value = %d\n", result);
            exit(-1);
        }
    }

    for (int i = 0; i < k; i++)
        pthread_join(thread[i], (void **) NULL);

    //PrintMatrix(A, n, "A");
    //PrintMatrix(B, n, "B");
    //PrintMatrix(C, n, "C");

    DeleteMatrix(A, n);
    DeleteMatrix(B, n);
    DeleteMatrix(C, n);
    return 0;
}
