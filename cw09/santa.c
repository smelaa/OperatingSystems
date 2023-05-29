#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>

#define ELF_N 3
#define ELF_N_TOTAL 9
#define RAINDEER_N 9

int elf_cnt=0;
int raindeer_cnt=0;
int santa_sleeping=1;

pthread_t elf_queue[ELF_N];

pthread_mutex_t elf_mutex;
pthread_mutex_t raindeer_mutex;
pthread_mutex_t santa_mutex;

pthread_cond_t santa_woken;
pthread_cond_t elf_problem;
pthread_cond_t raindeer_waiting;


void wake_santa(void){
    pthread_mutex_lock(&santa_mutex);
    santa_sleeping=0;
    pthread_cond_broadcast(&santa_woken);
    pthread_mutex_unlock(&santa_mutex);
}


void *elf_behavior(void *args){
    pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    while(1){
        sleep(rand()%4+2);                                                           //work
        pthread_mutex_lock(&elf_mutex); 
        if(elf_cnt<ELF_N){                                                           //go to Santa
            elf_queue[elf_cnt]=pthread_self();
            elf_cnt+=1;
            printf("Elf %lu: czeka %d elfów na Mikołaja\n", pthread_self(), elf_cnt);
            if(elf_cnt==ELF_N){                                                         //wake Santa
                printf("Elf %lu: wybudzam Mikołaja\n", pthread_self());
                pthread_mutex_unlock(&elf_mutex); 
                wake_santa();
            }
            else{
                while(elf_cnt!=0){
                    pthread_cond_wait(&elf_problem, &elf_mutex);
                }
                pthread_mutex_unlock(&elf_mutex); 
            }
        }
        else{                                                                       //solve problem on your own
            printf("Elf %lu: samodzielnie rozwiązuję swój problem\n", pthread_self());
            pthread_mutex_unlock(&elf_mutex); 
        }
        
    }
    return NULL;
}

void *reindeer_behavior(void *args){
    pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    while(1){
        sleep(rand()%6+5);                                                           //vacation
        pthread_mutex_lock(&raindeer_mutex);                                            
        raindeer_cnt+=1;                                                             //come back
        printf("Renifer %lu: czeka %d reniferów na Mikołaja\n", pthread_self(), raindeer_cnt);
        if(raindeer_cnt==RAINDEER_N){                                                        //wake Santa
            printf("Renifer %lu: wybudzam Mikołaja\n", pthread_self());
            pthread_mutex_unlock(&raindeer_mutex);
            wake_santa();
        }  
        while(raindeer_cnt!=0){
            pthread_cond_wait(&raindeer_waiting, &raindeer_mutex);
        }
        pthread_mutex_unlock(&raindeer_mutex);
        
    }
    return NULL;
}

void *santa_behavior(void *args){
    pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    
    return NULL;
}

int main()
{   
    //INIT
    srand(time(NULL)); 

    pthread_mutex_init(&elf_mutex, NULL);
    pthread_mutex_init(&raindeer_mutex, NULL);
    pthread_mutex_init(&santa_mutex, NULL);
    pthread_cond_init(&santa_woken, NULL); 
    pthread_cond_init(&raindeer_waiting, NULL); 
    pthread_cond_init(&elf_problem, NULL); 

    pthread_t *threads;

    threads=calloc(ELF_N_TOTAL+RAINDEER_N, sizeof(pthread_t));

    int index=0;
    for(int i=0;i<ELF_N_TOTAL;i++){
        pthread_create(threads+sizeof(pthread_t)*index, NULL, reindeer_behavior, NULL);
        index++;
    }

    for(int i=0;i<RAINDEER_N;i++){
        pthread_create(threads+sizeof(pthread_t)*index, NULL, elf_behavior, NULL);
        index++;
    }

    //SANTA
    int presents_cnt=0;
    while(presents_cnt<3){  
        pthread_mutex_lock(&santa_mutex); 
        while(santa_sleeping){
            pthread_cond_wait(&santa_woken, &santa_mutex);
        }
        pthread_mutex_lock(&elf_mutex); 
        pthread_mutex_lock(&raindeer_mutex); 
        printf("Mikołaj: budzę się\n");
        if(elf_cnt==ELF_N){                                                       
            printf("Mikołaj: rozwiązuję problemy elfów %lu, %lu, %lu\n", elf_queue[0], elf_queue[1], elf_queue[2]);
            sleep(rand()%2+1);
            elf_cnt=0;
            pthread_cond_broadcast(&elf_problem);
        }

        if(raindeer_cnt==RAINDEER_N){                                                       
            printf("Mikołaj: dostarczam zabawki %d raz\n", presents_cnt+1);
            sleep(rand()%3+2);
            presents_cnt++;
            raindeer_cnt=0;
            pthread_cond_broadcast(&raindeer_waiting);
        }
        pthread_mutex_unlock(&elf_mutex);
        pthread_mutex_unlock(&raindeer_mutex);
        
        printf("Mikołaj: zasypiam\n");
        santa_sleeping=1;
        pthread_mutex_unlock(&santa_mutex);
    }


    //END
    pthread_mutex_lock(&elf_mutex);
    pthread_mutex_lock(&raindeer_mutex);
    for (int i=0;i<ELF_N_TOTAL+RAINDEER_N;i++){
        pthread_cancel(*(threads+sizeof(pthread_t)*i));
    }
    free(threads);
    
    pthread_mutex_destroy(&elf_mutex);
    pthread_mutex_destroy(&raindeer_mutex);
    pthread_mutex_destroy(&santa_mutex);

    printf("Mikołaj: zabawki dostarczone, koniec swiat\n");
    exit(0);
}
