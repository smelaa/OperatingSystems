#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

int sig_recieved;

void handler(int signum) {
    sig_recieved = 1;
    printf("SENDER # Signal recieved.\n");
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "SENDER # Wrong number of arguments!\n");
        return 0;
    }

    int state;
    int catcher_pid = atoi(argv[1]);

    struct sigaction action;
        sigemptyset(&action.sa_mask);
        action.sa_handler = handler;
        sigaction(SIGUSR1, &action, NULL);

    for (int i = 2; i < argc; i++) {
        sig_recieved = 0;

        state = atoi(argv[i]);

        sigval_t sig_val = {state};
        sigqueue(catcher_pid, SIGUSR1, sig_val);
        printf("SENDER # Sended state: %d\n", state);

        while(!sig_recieved);
    }
}