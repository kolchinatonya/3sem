#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <float.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <ncurses.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define FIELD_SIZE 9
#define MINE_PERCENT 25
#define MAX_QUEUE_SIZE 2

typedef struct Cell
{
        int x;
        int y;
}Cell;

typedef struct Inf
{
    Cell open;
    int status;
}Inf;

typedef struct sockaddr_in Addr;

void SetAround(int** field, int n);
int** GenerateField(int n);
void PrintField(int** field, int n);
void ClearField(int** field, int n);
int AddClients(int* clients, int new_one);
void AcceptClient(int* new_one, int* lsock);
void CreateConnection(int* lsock, Addr* addr);
void SendFieldToClient(int** field, int* clients);
void PrepareForGame(int **field, int* clients, int* lsock);

int main()
{
    int** field;
    int clients[2] = {0, 0}, lsock = 0;
    PrepareForGame(field, clients, &lsock);
    Inf inf;
    int cur_player = 0;
    while (1)
    {
        recv(clients[cur_player], &inf, sizeof(inf), 0);
        send(clients[!cur_player], &inf, sizeof(inf), 0);
        if (inf.status == 1)
            cur_player = !cur_player;
        if ((inf.status == -1) || (inf.status == 2))
        {
            printf("THE GAME IS OVER!\n");
            break;
        }
    }
    close(clients[0]);
    close(clients[1]);
    close(lsock);
    return 0;

}



void SetAround(int** field, int n)
{
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if (field[i][j] == 9)
                for (int k = i - 1; k < i + 2; ++k)
                    for (int l = j - 1; l < j + 2; ++l)
                    {
                        if ((k >= n) || (l >= n) || (k < 0) || (l < 0))
                            continue;
                        if (field[k][l] != 9)
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
            res_field[i][j] = (random < MINE_PERCENT) ? 9 : 0;
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

int AddClients(int* clients, int new_one)
{
    if	(!clients[0])
    {
        clients[0] = new_one;
        return 1;
    }
    else if (!clients[1])
    {
        clients[1] = new_one;
        return 2;
    }
    else
        return 0;
}

void AcceptClient(int* new_one, int* lsock)
{
    if ((*new_one = accept(*lsock, NULL, NULL)) == -1)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
}




void CreateConnection(int* lsock, Addr* addr)
{
    *lsock = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(*lsock, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
    if (*lsock == -1)
    {
        perror("server: socket");
        exit(EXIT_FAILURE);
    }

    addr->sin_family = AF_INET;
    addr->sin_port = htons(51000);
    addr->sin_addr.s_addr = htons(INADDR_ANY);
    if (bind(*lsock, (struct sockaddr*) addr, sizeof(*addr)) == -1)
    {
        perror("server: bind");
        exit(EXIT_FAILURE);
    }
    listen(*lsock, MAX_QUEUE_SIZE);
}

void SendFieldToClient(int** field, int* clients)
{
    for (int k = 0; k < 2; ++k) {
        for (int i = 0; i < FIELD_SIZE; ++i) {
            for (int j = 0; j < FIELD_SIZE; ++j)
                send(clients[k], &field[i][j], sizeof(int), 0);
        }
    }
}


void PrepareForGame(int **field, int* clients, int* lsock)
{
    field = GenerateField(FIELD_SIZE);
    PrintField(field, FIELD_SIZE);

    int  new_one = 0;
    Addr addr = {};
    CreateConnection(lsock, &addr);

    while (1)
    {
        AcceptClient(&new_one, lsock);
        int clients_count = AddClients(clients, new_one);
        if (clients_count == 1)
        {
            int turn = 0;
            send(clients[0], &turn, sizeof(int), 0);
        }
        if (clients_count == 2)
        {
            int turn = 1;
            send(clients[1], &turn, sizeof(int), 0);
            SendFieldToClient(field,clients);
            return;
        }
    }

}




