#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <sys/sem.h>
#include <unistd.h>

#define LIMIT 2

void SemCreate(int *semid, int count)
{
    char pathname[] = "help_sem.c";

    key_t key;

    if((key = ftok(pathname,0)) < 0){
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if((*semid = semget(key, count, 0666 | IPC_CREAT)) < 0)
    {
        printf("Can\'t get semid\n");
        exit(-1);
    }
}


void SemOperation(int semid, short operation, unsigned short sem_mumber)
{
    struct sembuf mybuf;

    mybuf.sem_op = operation;
    mybuf.sem_flg = 0;
    mybuf.sem_num = sem_mumber;
    semop(semid, &mybuf, 1);
}


int main(void)
{
    int semid;
    SemCreate(&semid, 1);
    SemOperation(semid, LIMIT, 0);

    int msqid;
    char pathname[]="help.c";
    key_t  key;
    int len;

    struct mymsgbuf
    {
        long mtype;
        struct
        {
            int a;
            int b;
            int pid;
        }info;
    } msgbuf;

    struct myresbuf
    {
        long mtype;
        int c;
    } resbuf;

    key = ftok(pathname, 0);

    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0){
        printf("Can\'t get msqid\n");
        exit(-1);
    }

    while (1) {

        len = sizeof(msgbuf.info);

        if (msgrcv(msqid, &msgbuf, (size_t)len, 1, 0) < 0)
        {
            printf("Can\'t receive message from queue\n");
            exit(-1);
        }
        pid_t pid = fork();
        if ( pid == 0) {
            SemOperation(semid, -1, 0);
            int c = msgbuf.info.a * msgbuf.info.b;
            sleep(5);

            resbuf.mtype = msgbuf.info.pid;
            resbuf.c = c;

            len = sizeof(resbuf.c);

            if (msgsnd(msqid, &resbuf, len, 0) < 0) {
                printf("Can\'t send message to queue\n");
                msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
                exit(-1);
            }
            SemOperation(semid, 1, 0);
            exit(0);
        }
    }

    return 0;
}
