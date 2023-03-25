#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <linux/limits.h>
#include <string.h>
#include <zconf.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>


int search_dir(char* dir_path, char* str){
    DIR *dir;

    dir = opendir(dir_path);
    if (dir == NULL) {
        perror( "opendir" );
        exit(1);
    }

    struct dirent *file;
    struct stat f_stat;

    while ((file = readdir(dir)) != NULL) {
        //pomijam katalogi . i ..
        if (strcmp(file ->d_name, ".") == 0 || strcmp(file ->d_name, "..") == 0){
            continue;
        }

        char *file_path=malloc(PATH_MAX);
        strcpy(file_path, dir_path);
        strcat(file_path, "/");
        strcat(file_path, file->d_name);
        
        if (stat(file_path, &f_stat)==-1) {  
            fprintf(stderr, "stat %s: %s\n", file_path, strerror(errno));
            exit(1);
        }      
        //jesli plik to katalog
        if (S_ISDIR(f_stat.st_mode)){         
            int fpid=fork();
            if(fpid==-1){
                perror( "fork" );
                exit(1);
            } else if (fpid==0){
                search_dir(file_path, str);
                return 0;
            }
        }
        //jesli to nie katalog
        else{

            FILE* o_file;
			if ((o_file = fopen(file_path, "r")) == NULL) {
				perror("fopen");
				exit(1);
			}

            char *buff=malloc(sizeof(char)*strlen(str));

            fread(buff, sizeof(char),strlen(str), o_file);
            fclose(o_file);
            if (strncmp(buff, str, strlen(str)) == 0) {
				printf("%s\t\tPID: %d\n", file_path,getpid());
			} 

            free(buff);
        }
        free(file_path);
    }
    closedir(dir);
    return 0; 
}

int main(int argc, char* argv[]){
    if(argc!=3){
        fprintf(stderr, "wrong number of arguments\n");
        return -1;
    }
    char *path=argv[1];
    char *str=argv[2];
    if (strlen(str) > 255) {
        fprintf(stderr, "2nd argument to long.\n");
        return -1;
    } 

    char real_path[PATH_MAX];
    realpath(path, real_path);

    search_dir(real_path,str);

    while(wait(NULL)>0);

    return 0;
}