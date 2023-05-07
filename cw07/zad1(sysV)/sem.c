#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include "sem.h"

int create_sem(char *key_path, int key_id, int init_val) {
    key_t key = ftok(key_path, key_id);
    if (key == -1) {
        fprintf(stderr, "ERROR: Creating a semaphore failed on ftok.\n");
        return -1;
    }
    del_sem(semget(key, 0, 0)); //del if exists
    int sem_id = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (sem_id == -1) {
        fprintf(stderr, "ERROR: Creating a semaphore failed on semget.\n");
        return -1;
    }
    if(semctl(sem_id, 0, SETVAL, init_val) == -1) {
        fprintf(stderr, "ERROR: Creating a semaphore failed on semctl.\n");
        return -1;
    }
    return sem_id;
}

int open_sem(char *key_path, int key_id) {
    key_t key = ftok(key_path, key_id);
    if (key == -1) {
        fprintf(stderr, "ERROR: Opening a semaphore failed on ftok.\n");
        return -1;
    }
    int sem_id = semget(key, 0, 0);
    if (sem_id == -1) {
        fprintf(stderr, "ERROR: Opening a semaphore failed on semget.\n");
        return -1;
    }
    return sem_id;
}

void del_sem(int sem_id) {
    semctl(sem_id, 0, IPC_RMID);
}