#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef struct in_queue {
	int client_id;
    int hairstyle;
} in_queue;

int create_queue(void); 
int get_queue(void);
in_queue *attach_shm(int shm_id );
void detach_shm(void* shm_at );
void del_shm(int shm_id); 

#endif