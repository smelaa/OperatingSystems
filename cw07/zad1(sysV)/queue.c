#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include "heading.h"
#include "queue.h"

int create_queue(void) {
    key_t key = ftok(SHM_PATH, SHM_ID);
    if (key == -1) {
        fprintf(stderr, "ERROR: Creating shared memory failed on ftok.\n");
        return -1;
    }
    del_shm(shmget(key, 0,0));
    int shm_id=shmget(key, sizeof(in_queue)*N_QUEUE, IPC_CREAT | IPC_EXCL | 0644);
    if (shm_id == -1) {
        fprintf(stderr, "ERROR: Creating shared memory failed on shmget.\n");
        return -1;
    }
    return shm_id;
}

int get_queue(void) {
    key_t key = ftok(SHM_PATH, SHM_ID);
    if (key == -1) {
        fprintf(stderr, "ERROR: Getting shared memory failed on ftok.\n");
        return -1;
    }
    int shm_id=shmget(key, sizeof(in_queue)*N_QUEUE, 0644);
    if (shm_id == -1) {
        fprintf(stderr, "ERROR: Getting shared memory failed on shmget.\n");
        return -1;
    }
    return shm_id;
}

in_queue *attach_shm(int shm_id ) {
    in_queue *shm_at;
    if (shm_id == -1) {
        fprintf(stderr, "ERROR: -1 cannot be shared memory ID.\n");
        return NULL;
    }
    shm_at = shmat(shm_id, NULL, 0);
    if (shm_at == (in_queue*)(-1)) {
        fprintf(stderr, "ERROR: Failed to load block with ID %d.\n", shm_id);
        return NULL;
    }
    return shm_at;
}

void detach_shm(void* shm_at ) {
    if(shmdt(shm_at) == -1){
        fprintf(stderr, "ERROR: Failed to detach shm block.\n");
    }
}

void del_shm(int shm_id) {
    if(shmctl(shm_id, IPC_RMID, NULL) == -1){
        fprintf(stderr, "ERROR: Failed to delete shm block.\n");
    }
}