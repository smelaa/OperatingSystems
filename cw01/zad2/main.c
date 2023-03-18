#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <dlfcn.h>
#include <ctype.h>
#include "my_library.h"

void printTimes(struct tms cpu_time_start, struct tms cpu_time_end, clock_t real_time_start, clock_t real_time_end){
    double real_time, user_time, system_time;
    user_time=(double)(cpu_time_end.tms_utime-cpu_time_start.tms_utime)/sysconf(_SC_CLK_TCK);
    system_time=(double)(cpu_time_end.tms_stime-cpu_time_start.tms_stime)/sysconf(_SC_CLK_TCK);
    real_time=(double)(real_time_end-real_time_start)/CLOCKS_PER_SEC;
    printf("user time: %f s; system time: %f s; real time: %f s\n", user_time,system_time,real_time);
}

char* trim_end(char* str) {
    int len = strlen(str);
    
    while (len > 0 && isspace(str[len - 1])) {
        str[--len] = '\0';
    }
    
    return str;
}

int main() {
    #ifdef DLL
    void *handle = dlopen("../zad1/libmy_library.so", RTLD_LAZY);
    if(handle==NULL){
        fprintf(stderr, "could not find library.\n");
        return 1;
    }
    if(!handle){
        fprintf(stderr, "%s\n", dlerror());
        dlclose(handle);
        return 1;
    }

    pointersArray (*createPointersArray)(int) =dlsym(handle, "createPointersArray");
    int (*countLinesAndWords)(pointersArray*, char*) =dlsym(handle, "countLinesAndWords");
    char* (*returnBlock)(pointersArray*, int) =dlsym(handle, "returnBlock");
    int (*removeBlock)(pointersArray*, int) =dlsym(handle, "removeBlock");
    int (*removeArray)(pointersArray*) =dlsym(handle, "removeArray");

    if(dlerror() != NULL){
        fprintf(stderr, "%s\n", dlerror());
        dlclose(handle);
        return 1;
    }    
    #endif

    char* cmd;
    char* args;
    pointersArray array;

    int array_created=0;

    clock_t real_time_start, real_time_end;
    struct tms cpu_time_start, cpu_time_end;

    while(1){
        char text[100];
        printf("\n> ");
        if (fgets(text,sizeof(text), stdin)==NULL){continue;}
        cmd=strtok(text, " \n");
        trim_end(cmd);

        real_time_start=clock();
        times(&cpu_time_start);

        if(strcmp(cmd,"quit")==0){
            printf("ended.\n");
            break;
        }

        else if (strcmp(cmd,"init")==0){
            if(array_created!=0){
                printf("the array has been initialized already.\n");
                continue;
            }
            args=strtok(NULL, " \n");
            trim_end(args);
            if(args==NULL){
                printf("size not defined.\n");
                continue;
            }
            array=createPointersArray(atoi(args));
            array_created=1;
            printf("the array size %s initialized.\n", args);
        }

        else if (strcmp(cmd,"count")==0){
            args=strtok(NULL, " \n");
            trim_end(args);
            if(args==NULL){
                printf("file path not defined.\n");
                continue;
            }
            if(array_created==0){
                printf("you have to initialize an array first. use 'init [size]'.\n");
                continue;
            }
            if (countLinesAndWords(&array, args)==0){
                printf("wc completed on file %s.\n", args);
            }
            else {printf("error occured during executing wc on file %s.\n", args);}
        }

        else if (strcmp(cmd,"show")==0){
            args=strtok(NULL, " \n");
            trim_end(args);
            if(args==NULL){
                printf("index not defined.\n");
                continue;
            }
            if(array_created==0){
                printf("an array not initialized.\n");
                continue;
            }
            const char* block=returnBlock(&array, atoi(args));
            if (block==NULL){
                printf("error occured, could not show array at index %s\n", args);
                continue;
            }
            printf("in array at index %s:\n %s", args,block);
        }

        else if(strcmp(cmd,"delete")==0){
            args=strtok(NULL, " \n");
            trim_end(args);
            if(args==NULL){
                printf("index not defined.\n");
                continue;
            }
            if(array_created==0){
                printf("an array not initialized.\n");
                continue;
            }
            removeBlock(&array, atoi(args));
            printf("block number %i deleted.\n", atoi(args));
        }

        else if(strcmp(cmd,"destroy")==0){
            if(array_created==0){
                printf("an array not initialized.\n");
                continue;
            }
            removeArray(&array);
            array_created=0;
            printf("array deleted.\n");
        }
        else {
            printf("unknown command.\n");
            continue;
        }
        real_time_end=clock();
        times(&cpu_time_end);
        printTimes(cpu_time_start,cpu_time_end, real_time_start, real_time_end);        
    }
    #ifdef DLL
    dlclose(handle);
    #endif
    return 0;
}