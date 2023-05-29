#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>


const int grid_width = 30;
const int grid_height = 30;

char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}

void destroy_grid(char *grid)
{
    free(grid);
}

void destroy_threads(thr_struct *threads)
{
    for (int i=0; i < grid_height*grid_width; i++) {
        free((threads+i)->args);
        free((threads+i)->thread);
    }
    free(threads);
}

void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}


void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
}

thr_struct *init_threads(char *src, char *dst){
    int n=grid_height*grid_width;
    thr_struct *threads=malloc(sizeof(thr_struct)*n);

    for (int i = 0; i < grid_height; ++i)
    {
        for (int j = 0; j < grid_width; ++j)
        {
            thr_args *args = malloc(sizeof(thr_args));
            args->dst=dst;
            args->src=src;
            args->x=j;
            args->y=i;
            pthread_t *thr = malloc(sizeof(pthread_t));
            pthread_create(thr, NULL, update_grid_cell, (void*) args);
            (threads+(i * grid_width + j))->args=args;
            (threads+(i * grid_width + j))->thread=thr;

        }
    }

    return threads;
}

bool is_alive(int row, int col, char *grid)
{

    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}

void *update_grid_cell(void * args){
    thr_args* thargs= (thr_args*) args;
    while(1){
        pause();
        thargs->dst[thargs->y * grid_width + thargs->x] = is_alive(thargs->y, thargs->x, thargs->src);
        char* tmp = thargs->src;
		thargs->src = thargs->dst;
		thargs->dst = tmp;
    }
    return NULL;
}

void update_grid(thr_struct *threads)
{   
    for (int i=0; i < grid_height*grid_width; i++) {
        pthread_kill(*(threads+i)->thread, SIGUSR1);
    }
}
