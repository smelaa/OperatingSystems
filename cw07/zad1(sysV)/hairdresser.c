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


int main(void){
    //dolaczenie pamieci dzielonej i semaforow
    int queue_id=get_queue();
    void * queue_at = attach_shm(queue_id);
    if(queue_at== NULL) {
        fprintf(stderr, "ERROR: Failed to load block of shared memory.\n");
		return -1;
    }

    int hairdrs_sem=open_sem(HAIRDRS_SEM_PATH, HAIRDRS_SEM_ID);
    int chairs_sem=open_sem(CHAIRS_SEM_PATH, CHAIRS_SEM_ID);
    int queue_sem=open_sem(QUEUE_SEM_PATH, QUEUE_SEM_ID);
    int clients_sem=open_sem(CLIENTS_SEM_PATH, CLIENTS_SEM_ID);
    int ready_sem=open_sem(READY_SEM_PATH, READY_SEM_ID);

    struct sembuf operation_dec={ 0, -1, 0 };
    struct sembuf operation_inc={ 0, 1, 0 };

    while(1){
        printf("H %d\t|\tWaiting for customers.\n", getpid());
        //zmniejsz liczbe dostepnych foteli
        if(semop(chairs_sem, &operation_dec, 1)==-1){
            fprintf(stderr, "ERROR: chairs_sem semop.\n");
            return -1;
        }
        //zmniejsz liczbe czekajacych w kolejce
        if(semctl(clients_sem, 0, GETVAL)==0){
            //jezeli nie przyjdzie nowy klient to nie czekaj jesli nikogo nie ma w kolejce
            struct sembuf operation={ 0, -1, IPC_NOWAIT };
            if(semop(queue_sem, &operation, 1)==-1){
                fprintf(stderr, "ERROR: queue_sem semop.\n");
                break;
            }
        }
        else{
            if(semop(queue_sem, &operation_dec, 1)==-1){
                fprintf(stderr, "ERROR: queue_sem semop.\n");
                return -1;
            }
        }
        //ustaw flage - na razie nie mozna ruszac pamieci kolejki
        if(semop(ready_sem, &operation_dec, 1)==-1){
            fprintf(stderr, "ERROR: ready_sem semop.\n");
            return -1;
        }
        //zapros klienta z poczekalni na fotel
        in_queue client;
        memcpy(&client, queue_at, sizeof(in_queue));
        memcpy(queue_at, queue_at + sizeof(in_queue), semctl(queue_sem, 0, GETVAL)*sizeof(in_queue));
        //zmniejsz liczbe spiacych fryzjerow
        if(semop(hairdrs_sem, &operation_dec, 1)==-1){
            fprintf(stderr, "ERROR: hairdrs_sem semop.\n");
            return -1;
        }
        //ustaw flage - juz mozna ruszac pamiec kolejki
        if(semop(ready_sem, &operation_inc, 1)==-1){
            fprintf(stderr, "ERROR: ready_sem semop.\n");
            return -1;
        }
        
        //zrob fryzure
        printf("H %d\t|\tCustomer %d is being serviced (time: %ds).\n", getpid(), client.client_id, client.hairstyle);
        sleep(client.hairstyle);
        printf("H %d\t|\tCustomer %d hair done.\n", getpid(), client.client_id);

        //zwieksz liczbe dostepnych foteli
        if(semop(chairs_sem, &operation_inc, 1)==-1){
            fprintf(stderr, "ERROR: chairs_sem semop.\n");
            return -1;
        }
        //jezli wszyscy klienci beda obsluzeni to wracaj do domu
        if(semctl(hairdrs_sem, 0, GETVAL)>=semctl(clients_sem, 0, GETVAL)+semctl(queue_sem, 0, GETVAL)){
            break;
        }
        //zwieksz liczbe spiacych fryzjerow
        if(semop(hairdrs_sem, &operation_inc, 1)==-1){
            fprintf(stderr, "ERROR: hairdrs_sem semop.\n");
            return -1;
        }
    }
    

    //odlaczenie pamieci dzielonej
    detach_shm(queue_at);
    printf("H %d\t|\tDone.\n", getpid());
    return 0;
}