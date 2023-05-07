#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include<sys/mman.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "heading.h"
#include "queue.h"

int create_queue(void) {
    int shm_desc=shm_open(SHM_PATH, O_CREAT | O_RDWR | O_EXCL, 0644);
    if (shm_desc == -1) {
        fprintf(stderr, "ERROR: Creating shared memory failed on shm_open.\n");
        return -1;
    }
    if(ftruncate(shm_desc, N_QUEUE*sizeof(in_queue)) == -1) {
        fprintf(stderr, "ERROR: Creating shared memory failed on ftruncate.\n");
        return -1;
    }
    return shm_desc;
}

int get_queue(void) {
    int shm_desc=shm_open(SHM_PATH, O_RDWR, 0644);
    if (shm_desc == -1) {
        fprintf(stderr, "ERROR: Opening shared memory failed on shm_open.\n");
        return -1;
    }
    return shm_desc;
}

in_queue *attach_shm(int shm_desc ) {
    in_queue *shm_at;
    if (shm_desc == -1) {
        fprintf(stderr, "ERROR: -1 cannot be shared memory descriptor.\n");
        return NULL;
    }
    shm_at = (in_queue*) mmap(NULL, N_QUEUE*sizeof(in_queue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_desc, 0);
    return shm_at;
}

void detach_shm(void* shm_at ) {
    if(munmap(shm_at, N_QUEUE*sizeof(in_queue))== -1){
        fprintf(stderr, "ERROR: Failed to detach shm block.\n");
    }
}

void unlink_shm(void) {
    if(shm_unlink(SHM_PATH) == -1){
        fprintf(stderr, "ERROR: Failed to unlink shm block.\n");
    }
}