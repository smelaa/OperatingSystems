#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "heading.h"
#include "sem.h"
#include "queue.h"


int main(int argc, char** argv){
    if(argc!=2){
        fprintf(stderr, "ERROR: Wrong number of arguments. You should specify no. hairstyle (1-9).\n");
		return -1;
    }
    int hairstyle=atoi(argv[1]);
    if(hairstyle<1 || hairstyle>9){
        fprintf(stderr, "ERROR: Wrong argument. You should specify no. hairstyle (1-9).\n");
		return -1;
    }

    //dolaczenie semaforow
    int clients_sem=open_sem(CLIENTS_SEM_PATH, CLIENTS_SEM_ID);

    int id=N_CLIENTS-semctl(clients_sem, 0, GETVAL);
    printf("C %d\t|\tEntered.\n", id);
    if(id>=N_CLIENTS){
        printf("Daily limit of customers has been reached. Client left\n");
        return 0;
    }

    int queue_sem=open_sem(QUEUE_SEM_PATH, QUEUE_SEM_ID);
    int ready_sem=open_sem(READY_SEM_PATH, READY_SEM_ID);
    
    int place_tocpy=semctl(queue_sem, 0, GETVAL);
    if(place_tocpy>=N_QUEUE){
        printf("No place in waiting room. Client left.\n");
		return 0;
    }
    
    struct sembuf operation_dec = { 0, -1, 0 };
    struct sembuf operation_inc = { 0, 1, 0 };

    
    //ustaw flage - na razie nie mozna ruszac pamieci kolejki
    if(semop(ready_sem, &operation_dec, 1)==-1){
        fprintf(stderr, "ERROR: ready_sem semop.\n");
        return -1;
    }

    //dolaczanie pamieci dzielonej
    int queue_id=get_queue();
    void * queue_at = attach_shm(queue_id);
    if(queue_at== NULL) {
        fprintf(stderr, "ERROR: Failed to load block of shared memory.\n");
		return -1;
    }

    //klient siada w poczekalni
    in_queue cl_queue;
    cl_queue.client_id=id;
    cl_queue.hairstyle=hairstyle;
    
    if(semop(queue_sem, &operation_inc, 1)==-1){
        fprintf(stderr, "ERROR: queue_sem semop.\n");
		return -1;
    }
    
    if(semop(clients_sem, &operation_dec, 1)==-1){
        fprintf(stderr, "ERROR: clients_sem semop.\n");
		return -1;
    }

    memcpy(queue_at+place_tocpy*sizeof(in_queue), &cl_queue, sizeof(in_queue));

    //ustaw flage - juz mozna ruszac pamiec kolejki
    if(semop(ready_sem, &operation_inc, 1)==-1){
        fprintf(stderr, "ERROR: ready_sem semop.\n");
        return -1;
    }

    //odlaczenie pamieci dzielonej
    detach_shm(queue_at);

    printf("C %d\t|\tWaiting for hairstyle.\n", id);
    return 0;
}