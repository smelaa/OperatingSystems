
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

int cnt=0;
int done=1;
int state=0;

void handler(int signum, siginfo_t *info, void *context) {
	state = info->si_status;

	if (state < 1 || state > 5) {
		fprintf(stderr, "CATCHER # There is no option %d\n", state);
	}
	else {
		cnt++;
		done = 0;
	}

	kill(info->si_pid, SIGUSR1);
}

void print_numbers(){
    printf("CATCHER # Numbers 1-100:\n");
    for (int i=1;i<100;i++){
        printf("%d, ", i);
    }
    printf("100\n");
}

void print_curr_time(){
    time_t now = time(NULL);
	printf("CATCHER # Current time: %s\n", ctime(&now));
}

void print_req_num(){
    printf("CATCHER # Requests since start: %d\n", cnt);
}

void print_sec(){
    time_t now = time(NULL);
	printf("CATCHER # Current time: %s\n", ctime(&now));
    sleep(1);
}

void end_catching(){
    printf("CATCHER # END\n");
	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) {
	struct sigaction action;

	sigemptyset(&action.sa_mask);
	action.sa_sigaction = handler;
	action.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &action, NULL);

	printf("CATCHER # PID: %d\n", getpid());

	while(1) {
		if (done) pause();

		if (state == 1) {
            print_numbers();
            done=1;
		}
		else if (state == 2) {
            print_curr_time();
            done=1;
		}
		else if (state == 3) {
            print_req_num();
            done=1;
		}
		else if (state == 4) {
            print_sec();
		}
		else if (state == 5) {
            end_catching();
            done=1;
		}
        

	}

	return 0;
}