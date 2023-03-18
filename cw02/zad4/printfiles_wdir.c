#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ftw.h>

long long t_size=0;

int count(const char *path, const struct stat *file_stat, int typefile __attribute__((unused))){
    if (!S_ISDIR(file_stat->st_mode)){
        t_size += file_stat->st_size;
        printf("name: %s\t size: %lld\n", path, (long long)file_stat->st_size);
    }
    return 0;
}

int main(int argc, char* argv[]){
    if(argc!=2){
        fprintf(stderr, "Wrong number of arguments\n");
        return -1;
    }

    if(ftw(argv[1], count, 15)==-1){
        fprintf(stderr, "ftw errot\n");
        return -1;
    }

    printf("total size in bytes: %lld\n", t_size);
    return 0;
}