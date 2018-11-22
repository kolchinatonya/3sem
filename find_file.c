#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

char* DefineType(struct stat* file_stat)
{
    char* type = "no_name";
    if (S_ISREG(file_stat->st_mode)) type = "regular_file";
    if (S_ISDIR(file_stat->st_mode)) type = "directory";
    return type;
}

void Find(char* current_dir, int depth, char* file_name, int max_depth)
{
    if (depth > max_depth)
        return;
    DIR* dir = opendir(current_dir);
    struct dirent* entry;
    struct stat file_stat;
    if (!dir) {
        perror("diropen");
        exit(1);
    };

    while ( (entry = readdir(dir)) != NULL)
    {
        char* path = (char*)malloc(sizeof(char) * 1000);
        strcpy(path, current_dir);
        char* name = entry->d_name;
        path = strcat(path, "/");
        path = strcat(path, name);
        stat(path, &file_stat);
        char* type = DefineType(&file_stat);
        if (!strcmp(type,"regular_file"))
        {
            if (!strcmp(name, file_name))
                printf("%s\n",path);
        }
        if (!strcmp(type,"directory"))
        {
            bool flag = (!(strcmp(name, ".")) || !strcmp(name, ".."));
            if (!flag)
            {
                Find(path, depth + 1, file_name, max_depth);
            }
        }
        free(path);
    }
    closedir(dir);
}

int main(int argc, char** argv) {
    char* start_path = argv[1];
    int max_depth = atoi(argv[2]);
    char* file_name = argv[3];
    Find(start_path, 0, file_name, max_depth);
};