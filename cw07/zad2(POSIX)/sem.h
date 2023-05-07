#ifndef __SEM_H__
#define __SEM_H__

sem_t *create_sem(char *key_path, int init_val);
sem_t *open_sem(char *key_path);
void close_sem(sem_t* sem_p);
void del_sem(char* key_path);

#endif