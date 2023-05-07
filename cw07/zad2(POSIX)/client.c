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
    sem_t* clients_sem=open_sem(CLIENTS_SEM_PATH);
    if(clients_sem==NULL){
        fprintf(stderr, "ERROR: Salon is closed.\n");
		return -1;
    }

    int semvalue;
    sem_getvalue(clients_sem, &semvalue);
    int id=N_CLIENTS-semvalue;
    printf("C %d\t|\tEntered.\n", id);
    if(id>=N_CLIENTS){
        printf("Daily limit of customers has been reached. Client left\n");
        return 0;
    }

    sem_t* queue_sem=open_sem(QUEUE_SEM_PATH);
    if(queue_sem==NULL){
        fprintf(stderr, "ERROR: Salon is closed.\n");
		return -1;
    }
    sem_t* ready_sem=open_sem(READY_SEM_PATH);
    if(ready_sem==NULL){
        fprintf(stderr, "ERROR: Salon is closed.\n");
		return -1;
    }
    
    int place_tocpy;
    sem_getvalue(queue_sem, &place_tocpy);
    if(place_tocpy>=N_QUEUE){
        printf("No place in waiting room. Client left.\n");
		return 0;
    }

    
    //ustaw flage - na razie nie mozna ruszac pamieci kolejki
    if(sem_wait(ready_sem)==-1){
        fprintf(stderr, "ERROR: ready_sem sem_wait.\n");
        return -1;
    }

    //dolaczanie pamieci dzielonej
    int queue_desc=get_queue();
    void * queue_at = attach_shm(queue_desc);
    if(queue_at== NULL) {
        fprintf(stderr, "ERROR: Failed to load block of shared memory.\n");
		return -1;
    }

    //klient siada w poczekalni
    in_queue cl_queue;
    cl_queue.client_id=id;
    cl_queue.hairstyle=hairstyle;
    
    if(sem_post(queue_sem)==-1){
        fprintf(stderr, "ERROR: queue_sem sem_post.\n");
        return -1;
    }
    
    if(sem_wait(clients_sem)==-1){
        fprintf(stderr, "ERROR: clients_sem sem_wait.\n");
        return -1;
    }

    memcpy(queue_at+place_tocpy*sizeof(in_queue), &cl_queue, sizeof(in_queue));

    //ustaw flage - juz mozna ruszac pamiec kolejki
    if(sem_post(ready_sem)==-1){
        fprintf(stderr, "ERROR: ready_sem sem_post.\n");
        return -1;
    }

    //zamykanie semaforow
    close_sem(queue_sem);
    close_sem(clients_sem); 
    close_sem(ready_sem);

    //odlaczenie pamieci dzielonej
    detach_shm(queue_at);

    printf("C %d\t|\tWaiting for hairstyle.\n", id);
    return 0;
}