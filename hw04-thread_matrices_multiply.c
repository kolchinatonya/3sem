#include <stdio.h>
#include <time.h>
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

int main()
{
    int k, n;
    printf("Input the number of threads (k):\n");
    scanf("%d", &k);
    printf("Input the size of square matrix (n) (n %% k == 0 !):\n");
    scanf("%d", &n);

    if (n % k != 0)
    {
        printf("ERROR! (%d %% %d != 0) !", n, k);
        exit(1);
    }

    int **A = GenerateRandomMatrix(n);
    int **B = GenerateRandomMatrix(n);
    int **C = GenerateZeroMatrix(n);

    pthread_t thread[k];

    ParamsForThreadMultiply params = {A, B, C, n, k};

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

    PrintMatrix(A, n, "A");
    PrintMatrix(B, n, "B");
    PrintMatrix(C, n, "C");
    DeleteMatrix(A, n);
    DeleteMatrix(B, n);
    DeleteMatrix(C, n);
    return 0;
}
