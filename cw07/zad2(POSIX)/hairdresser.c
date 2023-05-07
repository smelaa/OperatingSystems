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


int main(void){
    //dolaczenie pamieci dzielonej i semaforow
    int queue_desc=get_queue();
    void * queue_at = attach_shm(queue_desc);
    if(queue_at== NULL) {
        fprintf(stderr, "ERROR: Failed to load block of shared memory.\n");
		return -1;
    }

    sem_t *hairdrs_sem=open_sem(HAIRDRS_SEM_PATH);
    sem_t *chairs_sem=open_sem(CHAIRS_SEM_PATH);
    sem_t *queue_sem=open_sem(QUEUE_SEM_PATH);
    sem_t *clients_sem=open_sem(CLIENTS_SEM_PATH);
    sem_t *ready_sem=open_sem(READY_SEM_PATH);

    int semvalue;
    int semvalue1;
    int semvalue2;

    while(1){
        printf("H %d\t|\tWaiting for customers.\n", getpid());
        //zmniejsz liczbe dostepnych foteli
        if(sem_wait(chairs_sem)==-1){
            fprintf(stderr, "ERROR: chairs_sem sem_wait.\n");
            return -1;
        }
        //zmniejsz liczbe czekajacych w kolejce
        sem_getvalue(clients_sem, &semvalue);
        if(semvalue==0){
            //jezeli nie przyjdzie nowy klient to nie czekaj jesli nikogo nie ma w kolejce
            if(sem_trywait(queue_sem)==-1){
                fprintf(stderr, "ERROR: queue_sem sem_trywait.\n");
                break;
            }
        }
        else{
            if(sem_wait(queue_sem)==-1){
                fprintf(stderr, "ERROR: queue_sem sem_wait.\n");
                return -1;
            }
        }
        //ustaw flage - na razie nie mozna ruszac pamieci kolejki
        if(sem_wait(ready_sem)==-1){
            fprintf(stderr, "ERROR: ready_sem sem_wait.\n");
            return -1;
        }
        //zapros klienta z poczekalni na fotel
        in_queue client;
        memcpy(&client, queue_at, sizeof(in_queue));
        sem_getvalue(queue_sem, &semvalue);
        memcpy(queue_at, queue_at + sizeof(in_queue), semvalue*sizeof(in_queue));
        //zmniejsz liczbe spiacych fryzjerow
        if(sem_wait(hairdrs_sem)==-1){
            fprintf(stderr, "ERROR: hairdrs_sem sem_wait.\n");
            return -1;
        }
        //ustaw flage - juz mozna ruszac pamiec kolejki
        if(sem_post(ready_sem)==-1){
            fprintf(stderr, "ERROR: ready_sem sem_post.\n");
            return -1;
        }
        
        //zrob fryzure
        printf("H %d\t|\tCustomer %d is being serviced (time: %ds).\n", getpid(), client.client_id, client.hairstyle);
        sleep(client.hairstyle);
        printf("H %d\t|\tCustomer %d hair done.\n", getpid(), client.client_id);

        //zwieksz liczbe dostepnych foteli
        if(sem_post(chairs_sem)==-1){
            fprintf(stderr, "ERROR: chairs_sem sem_post.\n");
            return -1;
        }
        //jezli wszyscy klienci beda obsluzeni to wracaj do domu
        sem_getvalue(hairdrs_sem, &semvalue);
        sem_getvalue(clients_sem, &semvalue1);
        sem_getvalue(queue_sem, &semvalue2);
        if(semvalue>=semvalue1+semvalue2){
            break;
        }
        //zwieksz liczbe spiacych fryzjerow
        if(sem_post(hairdrs_sem)==-1){
            fprintf(stderr, "ERROR: hairdrs_sem sem_post.\n");
            return -1;
        }
    }

    //zamykanie semaforow
    close_sem(hairdrs_sem);
    close_sem(chairs_sem);
    close_sem(queue_sem);
    close_sem(clients_sem); 
    close_sem(ready_sem);

    //odlaczenie pamieci dzielonej
    detach_shm(queue_at);

    printf("H %d\t|\tDone.\n", getpid());
    return 0;
}