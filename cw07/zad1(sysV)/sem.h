#ifndef __SEM_H__
#define __SEM_H__

int create_sem(char *key_path, int key_id, int init_val);
int open_sem(char *pathname, int key_id);
void del_sem(int sem_id);

#endif