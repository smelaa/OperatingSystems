#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include "heading.h"
#include "queue.h"
#include "sem.h"

int main(void){
    //tworzenie  shm
    int queue_id=create_queue();
    in_queue * queue_at = attach_shm(queue_id);
    if(queue_at == NULL) {
        fprintf(stderr, "ERROR: Failed to load block of shared memory.\n");
		return -1;
    }
    printf("START..\t|\tWaiting room (shm) ready.\n");

    //tworzenie sem
    int hairdrs_sem=create_sem(HAIRDRS_SEM_PATH, HAIRDRS_SEM_ID, N_HAIRDRS);
    int chairs_sem=create_sem(CHAIRS_SEM_PATH, CHAIRS_SEM_ID, N_CHAIRS);
    int queue_sem=create_sem(QUEUE_SEM_PATH, QUEUE_SEM_ID, 0);
    int clients_sem=create_sem(CLIENTS_SEM_PATH, CLIENTS_SEM_ID, N_CLIENTS);
    int ready_sem=create_sem(READY_SEM_PATH, READY_SEM_ID, 1);
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
    del_shm(queue_id);
    printf("END..\t|\tWaiting room closed (shm released).\n");

    //usuwanie semaforow
    del_sem(hairdrs_sem);
    del_sem(chairs_sem);
    del_sem(queue_sem);
    del_sem(clients_sem); 
    del_sem(ready_sem);
    printf("END..\t|\tReception closed (sem deleted).\n");

    printf("ENDED\t|\tSimulation finished.\n");
    return 0;
}