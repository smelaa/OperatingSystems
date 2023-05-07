#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include<sys/mman.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "sem.h"

sem_t *create_sem(char *key_path, int init_val) {
    sem_t *sem_p=sem_open(key_path, O_CREAT | O_EXCL, 0666,init_val);
    if(sem_p == SEM_FAILED){
        fprintf(stderr, "ERROR: Creating semaphore failed.\n");
        return NULL;
    }
    return sem_p;
}

sem_t *open_sem(char *key_path) {
    sem_t *sem_p=sem_open(key_path, 0);
    if(sem_p == SEM_FAILED){
        fprintf(stderr, "ERROR: Opening a semaphore failed.\n");
        return NULL;
    }
    return sem_p;
}

void close_sem(sem_t* sem_p) {
    sem_close(sem_p);
}

void del_sem(char* key_path) {
    sem_unlink(key_path);
}