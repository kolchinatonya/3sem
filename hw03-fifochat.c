
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_STRING_SIZE 200


int main(int argc, char *argv[])
{
    int to_read, to_write, fd;
    pid_t result;
    char* string;
    int ind = atoi(argv[1]);
    char* fifo[2];
    if (ind == 0)
    {
        to_read = 0;
        to_write = 1;
    }
    else
    {
        to_read = 1;
        to_write = 0;
    }

    fifo[to_read] = "1.fifo";
    fifo[to_write] = "2.fifo";

    /// это можно было бы написать короче
    /*
    fifo[ind] = "1.fifo";
    fifo[1 - ind] = "2.fifo";
    */
    
    // два последующие mknod одинаковые ... можно сделать цикл от 0 до 1
    if ((mknod(fifo[0], S_IFIFO | 0666, 0) < 0) && (errno != EEXIST))
    {
        printf("Can\'t create FIFO\n");
        exit(-1);
    }


    if ((mknod(fifo[1], S_IFIFO | 0666, 0) < 0) && (errno != EEXIST))
    {

        perror("i");
        printf("Can\'t create FIFO\n");
        exit(-1);
    }

    if((result = fork()) < 0)
    {
        exit(-1);
    }
    if (result > 0)
    {
        fd = open(fifo[1], O_WRONLY);
        if (fd  == -1)
        {
            printf("Can\'t open FIFO for writting\n");
            exit(-1);
        }
        while(1)
        {
            // кажется лишним каждый раз в цикле выделять и освобождать память под строку.
            // можно выделить память 1 раз до цикла
            string = (char*)calloc(MAX_STRING_SIZE, sizeof(char));
            fgets(string, MAX_STRING_SIZE, stdin);
            write(fd, string, strlen(string));
            free(string);
        }
        close(fd);

    }
    else
    {
        fd = open(fifo[0], O_RDONLY);
        if (fd  == -1)
        {
            printf("Can\'t open FIFO for reading\n");
            exit(-1);
        }
        while(1)
        {
            // аналогично память можно выделить 1 раз до цикла
            string = (char*)calloc(MAX_STRING_SIZE, sizeof(char));
            int size = read(fd, string, MAX_STRING_SIZE);
            if (size == 0)
            {
                printf("Another program exited\n");
                free(string);
                exit(0);
            }
            printf("- %s", string);
            free(string);
        }
        close(fd);
    }
    return 0;
}
