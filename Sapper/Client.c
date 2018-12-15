#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include <stdbool.h>


#define FIELD_SIZE 9

typedef struct sockaddr_in Addr;

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

int mine_count, opened_count = 0;

void CheckIP(int argc);
void Hello();
void CreateNetConnection(char** IP, int* sockfd, Addr  servaddr);
void GetFieldFromServer(int sockfd, int** server_field);
void CountMines(int** server_field);
char** GenerateEmptyField(int n);
int** GenerateEmptyIntField(int n);
void PrintIntField(int** field, int n);
void Wait(int sockfd, int** server_field, char** my_field);
void Play(int sockfd, int** server_field, char** my_field);
int Action(Cell* open, int** server_field, char** my_field);
int CheckBounds(Cell open);
int OpenCell(int** server_field, char** my_field, Cell* to_open, int val);
void OpenAllCells(int** server_field, char** my_field);
void PrintFieldWhithIndexes(char** field, int n);
void ClearField(char** field, int n);
void ClearIntField(int** field, int n);
void GameOver(int** server_field, char** my_field, int sockfd);
void Win(int** server_field, char** my_field, int sockfd);
void Exit(int** server_field, char** my_field, int sockfd);


int main(int argc, char **argv) {

    CheckIP(argc);
    Hello();

    int sockfd;
    Addr servaddr;

    CreateNetConnection(argv, &sockfd, servaddr);

    int **server_field = GenerateEmptyIntField(FIELD_SIZE);
    char **my_field = GenerateEmptyField(FIELD_SIZE);

    int my_turn;
    recv(sockfd, &my_turn, sizeof(int), 0);
    if (my_turn == 0) {
        printf("Sorry! You are alone here :( Waiting for the second player...\n");
        GetFieldFromServer(sockfd, server_field);
        PrintFieldWhithIndexes(my_field, FIELD_SIZE);
        Play(sockfd,server_field, my_field);
    } else {
        GetFieldFromServer(sockfd, server_field);
        Wait(sockfd, server_field, my_field);
        Play(sockfd,server_field, my_field);
    }
    return 0;
}






void CheckIP(int argc)
{
    if(argc != 2)
    {
        printf("Where is IP, man???? Bye! :p\n");
        exit(-1);
    }
}

void Hello()
{
    printf("\nHello! Do you want to play a new game? :) (y/n) \n");
    char answer;
    scanf("%c", &answer);
    if (answer == 'n')
    {
        printf("\nOK! Bye-bye ;) \n");
        exit(0);
    }
    if (answer == 'y')
    {
        return;
    }
    printf("\nYou are a stupid guy. I don't want to deal with you :( Bye!\n");
    exit(0);
}

void CreateNetConnection(char** IP, int* sockfd, Addr  servaddr)
{
    if ((*sockfd = socket (PF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("client: socket");
        exit(1);
    }
    servaddr.sin_family	= AF_INET;
    servaddr.sin_port = htons(51000);
    if (inet_aton(IP[1], &servaddr.sin_addr) == -1)
    {
        perror("inet_aton");
        exit(EXIT_FAILURE);
    }
    if (connect(*sockfd, (struct sockaddr*)&servaddr, sizeof(Addr)) == -1)
    {
        perror("client: connect");
        close(*sockfd);
        exit(1);
    }
}

void GetFieldFromServer(int sockfd, int** server_field)
{
    for (int i = 0; i < FIELD_SIZE; ++i) {
        for (int j = 0; j < FIELD_SIZE; ++j)
            recv(sockfd, &server_field[i][j], sizeof(int), 0);
    }
    CountMines(server_field);
}


void CountMines(int** server_field)
{
    for (int i = 0; i < FIELD_SIZE; ++i) {
        for (int j = 0; j < FIELD_SIZE; ++j) {
            if (server_field[i][j] == 9)
                mine_count++;
        }
    }

}


void Wait(int sockfd, int** server_field, char** my_field)
{
    system("clear");
    PrintFieldWhithIndexes(my_field, FIELD_SIZE);
    Inf inf;
    printf("Please, wait for your turn.\n");
    while (1)
    {
        recv(sockfd, &inf, sizeof(inf), 0);
        if (inf.status == 0)
        {
            int val = server_field[inf.open.x][inf.open.y];
            OpenCell(server_field, my_field, &inf.open, val);
            system("clear");
            PrintFieldWhithIndexes(my_field, FIELD_SIZE);
            printf("Please, wait for your turn.\n");
            continue;
        } else if (inf.status == 1)
        {
            int val = server_field[inf.open.x][inf.open.y];
            OpenCell(server_field, my_field, &inf.open, val);
            return;
        }
        else if (inf.status == -1)
        {
            OpenAllCells(server_field, my_field);
            GameOver(server_field, my_field, sockfd);
        } else
        {
            OpenAllCells(server_field, my_field);
            Win(server_field, my_field, sockfd);
        }
    }

}

void Win(int** server_field, char** my_field, int sockfd)
{
    system("clear");
    PrintFieldWhithIndexes(my_field, FIELD_SIZE);
    printf("YOU WIN! :)\n");
    Exit(server_field, my_field, sockfd);
}

void GameOver(int** server_field, char** my_field, int sockfd)
{
    system("clear");
    PrintFieldWhithIndexes(my_field, FIELD_SIZE);
    printf("YOU EXPLODED! :(\n");
    Exit(server_field, my_field, sockfd);
}

void Exit(int** server_field, char** my_field, int sockfd)
{
    close(sockfd);
    ClearIntField(server_field, FIELD_SIZE);
    ClearField(my_field, FIELD_SIZE);
    exit(0);
}

void Play(int sockfd, int** server_field, char** my_field) {
    while(1) {
        system("clear");
        PrintFieldWhithIndexes(my_field, FIELD_SIZE);
        char command;
        Cell cur = {};
        printf("Put your command: (< O x y > or < F x y > or < N x y >)\n");
        if ((scanf("%c %d %d", &command, &cur.x, &cur.y)) != 3)
            continue;
        cur.x--;
        cur.y--;
        if (CheckBounds(cur))
            continue;
        if (command == 'F')
            my_field[cur.x][cur.y] = '%';
        if (command == 'N')
        {
            if (my_field[cur.x][cur.y] == '%')
                my_field[cur.x][cur.y] = '*';
        }
        if (command == 'O')
        {
            int status = Action(&cur, server_field, my_field);
            Inf inf = {cur, status};
            send(sockfd, &inf, sizeof(inf), 0);
            switch(status)
            {
                case -1:
                    GameOver(server_field, my_field, sockfd);
                case 0:
                    continue;
                case 1:
                    Wait(sockfd, server_field, my_field);
                    break;
                default:
                    OpenAllCells(server_field,my_field);
                    Win(server_field, my_field, sockfd);
            }
        }
    }
}

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

int** GenerateEmptyIntField(int n)
{
    int** res_field = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
    {
        res_field[i] = (int *)malloc(n * sizeof(int));
        for (int j = 0; j < n; j++)
        {
            res_field[i][j] =  0;
        }
    }
    return res_field;
}

void PrintIntField(int** field, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
            printf("%2d ", field[i][j]);
        printf("\n");
    }
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

int OpenCell(int** server_field, char** my_field, Cell* to_open, int val)
{
    int status = 0;
    my_field[to_open->x][to_open->y] = val + '0';
    opened_count++;
    if (opened_count == FIELD_SIZE * FIELD_SIZE - mine_count)
        status = 1;
    return status;
}

void OpenAllCells(int** server_field, char** my_field) {
    for (int i = 0; i < FIELD_SIZE; ++i) {
        for (int j = 0; j < FIELD_SIZE; ++j) {
            if (server_field[i][j] == 9)
                my_field[i][j] = '@';
            else {
                int val = server_field[i][j];
                Cell to_open = {i, j};
                OpenCell(server_field, my_field, &to_open, val);
            }
        }
    }
}
void ClearField(char** field, int n)
{
    for (int i = 0; i < n; i++)
        free(field[i]);
    free(field);
}

void ClearIntField(int** field, int n)
{
    for (int i = 0; i < n; i++)
        free(field[i]);
    free(field);
}

int CheckBounds(Cell open)
{
    if ((open.x >= FIELD_SIZE) || (open.x < 0 ) || (open.y >= FIELD_SIZE) || (open.y < 0 ))
    {

        printf("How you can be so stupid? Where is that cell here??? Try again!\n");
        sleep(2);
        return 1;
    }
    return 0;
}

int Action(Cell* open, int** server_field, char** my_field)
{
    int val = server_field[open->x][open->y];
    if ( val == 9)
    {
        OpenAllCells(server_field,my_field);
        return -1;
    }
    else if (val == 0)
    {
        int status = OpenCell(server_field, my_field, open, val);
        if (status == 1)
            return 2;
        return 0;
    }
    else
    {
        int status = OpenCell(server_field, my_field, open, val);
        if (status == 1)
            return 2;
        return 1;
    }

}
