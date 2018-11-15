#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char * argv[])
{

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
        long restype;
        int c;
    } resbuf;

    key = ftok(pathname, 0);

    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0){
        printf("Can\'t get msqid\n");
        exit(-1);
    }


    msgbuf.mtype = 1;
    msgbuf.info.a = atoi(argv[1]);
    msgbuf.info.b = atoi(argv[2]);
    msgbuf.info.pid = getpid();

    len = sizeof(msgbuf.info);

    if (msgsnd(msqid, &msgbuf, len, 0) < 0)
    {
        printf("Can\'t send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(-1);
    }

    while (1) {

        len = sizeof(msgbuf.info);

        if (msgrcv(msqid, &resbuf, (size_t)len, getpid(), 0) < 0)
        {
            printf("Can\'t receive message from queue\n");
            exit(-1);
        }

        printf("%d * %d = %d\n",msgbuf.info.a, msgbuf.info.b, resbuf.c);


    }

    return 0;
}
