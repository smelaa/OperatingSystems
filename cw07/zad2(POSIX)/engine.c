#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include<sys/mman.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#include "heading.h"
#include "queue.h"
#include "sem.h"

int main(void){
    //tworzenie  shm
    int queue_desc=create_queue();
    in_queue * queue_at = attach_shm(queue_desc);
    if(queue_at == NULL) {
        fprintf(stderr, "ERROR: Failed to load block of shared memory.\n");
		return -1;
    }
    printf("START..\t|\tWaiting room (shm) ready.\n");

    //tworzenie sem
    sem_t *hairdrs_sem=create_sem(HAIRDRS_SEM_PATH, N_HAIRDRS);
    sem_t *chairs_sem=create_sem(CHAIRS_SEM_PATH, N_CHAIRS);
    sem_t *queue_sem=create_sem(QUEUE_SEM_PATH, 0);
    sem_t *clients_sem=create_sem(CLIENTS_SEM_PATH, N_CLIENTS);
    sem_t *ready_sem=create_sem(READY_SEM_PATH, 1);
    printf("START..\t|\tReception (sems) ready.\n");

    //uruchomienie procesow fryzjerow
    for (int i=0;i<N_HAIRDRS;i++){
        if (fork() == 0) {
            if(execl(HAIRDR_EXEC, HAIRDR_EXEC, NULL)==-1){
                fprintf(stderr, "ERROR: Problem with execl hairdressers.\n");
                return -1;
            }
        }
    }
    printf("START..\t|\tAll hairdressers ready.\n");

    //kumunikat startu
    printf("STARTED\t|\thairdressers: %d; \n\t\tchairs: %d; \n\t\tmax. queue size: %d; \n\t\tclients: %d.\n", N_HAIRDRS, N_CHAIRS, N_QUEUE, N_CLIENTS);

    while(wait(NULL)>0);

    //usuwanie pamieci dzielonej
    detach_shm(queue_at);
    unlink_shm();
    printf("END..\t|\tWaiting room closed (shm released).\n");

    //usuwanie semaforow
    close_sem(hairdrs_sem);
    close_sem(chairs_sem);
    close_sem(queue_sem);
    close_sem(clients_sem); 
    close_sem(ready_sem);

    del_sem(HAIRDRS_SEM_PATH);
    del_sem(CHAIRS_SEM_PATH);
    del_sem(QUEUE_SEM_PATH);
    del_sem(CLIENTS_SEM_PATH); 
    del_sem(READY_SEM_PATH);
    printf("END..\t|\tReception closed (sem deleted).\n");

    printf("ENDED\t|\tSimulation finished.\n");
    return 0;
}