#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

int main(){
    DIR* dir = opendir(".");
    if (dir == NULL){
        fprintf(stderr, "could not open current directory\n");
        return -1;
    }

    struct dirent *file;
    struct stat file_stat;
    long long t_size=0;
    

    while( (file=readdir(dir)) != NULL){
        if(stat(file -> d_name, &file_stat)!=0){
            fprintf(stderr, "stat error\n");
            return -1;
        }
        else if (!S_ISDIR(file_stat.st_mode)){
            t_size += file_stat.st_size;
            printf("name: %s\t size: %lld\n", file->d_name, (long long)file_stat.st_size);
        }
    }
    printf("total size in bytes: %lld\n", t_size);
    closedir(dir);
    return 0;
}