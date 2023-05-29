#pragma once
#include <stdbool.h>
#include <pthread.h>

typedef struct thr_args {
	int ix;
    int n;
    char *src; 
    char *dst;
} thr_args;

typedef struct thr_struct {
	pthread_t *thread;
    thr_args *args;
} thr_struct;

char *create_grid();
void destroy_grid(char *grid);
void destroy_threads(thr_struct *threads, int n);
void draw_grid(char *grid);
void init_grid(char *grid);
thr_struct *init_threads(char *src, char *dst, int n);
bool is_alive(int row, int col, char *grid);
void update_grid(thr_struct *threads, int n);
void* update_grid_cell(void * args);