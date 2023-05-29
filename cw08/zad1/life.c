#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>

void ignore_handler(int signo, siginfo_t* info, void* context) {}

int main()
{
	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	char *foreground = create_grid();
	char *background = create_grid();
	thr_struct * threads=init_threads(foreground, background);

	init_grid(foreground);

	struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = ignore_handler;
    sigaction(SIGUSR1, &action, NULL);

	while (true)
	{
		draw_grid(foreground);
		usleep(500 * 1000);

		// Step simulation
		update_grid(threads);
	}

	endwin(); // End curses mode
	destroy_threads(threads);
	destroy_grid(foreground);
	destroy_grid(background);

	return 0;
}
