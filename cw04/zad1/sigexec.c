#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

int is_parent=0;

void raise_signal(void){
    printf("PID: %d\traised signal\n", getpid());
    raise(SIGUSR1);
}

int mask_signal(void){
    sigset_t newmask;/* sygnały do blokowania */
    sigemptyset(&newmask); /* wyczyść zbiór blokowanych sygnałów */
    sigaddset(&newmask, SIGUSR1); /* dodaj SIGUSR1 do zbioru */
    /* Dodaj do zbioru sygnałów zablokowanych */
    if (sigprocmask(SIG_BLOCK, &newmask, NULL) < 0){
        perror("Could not set mask\n");
        return 1;
    }
    return 0;
}

int check_pending(void){
    sigset_t pen_set;
    if (sigpending(&pen_set)<0){
        perror("Could not get set of pending signals\n");
        return 1;
    }
    if (sigismember(&pen_set, SIGUSR1)) {
        printf("PID: %d\tSIGUSR1 is pending\n", getpid());
    }
    else {
        printf("PID: %d\tSIGUSR1 is not pending\n", getpid());
    }
    return 0;
}

int handle_signal(int is_root, char* option){
    if (strcmp(option, "ignore") == 0) {
            if(is_parent){
                signal(SIGUSR1, SIG_IGN);
            }
            raise_signal();
        }
        else if (strcmp(option, "mask") == 0) {
            if(is_parent){
                mask_signal();
            }
            raise_signal();
        }
        else if (strcmp(option, "pending") == 0) {
            if(is_parent){
                mask_signal();
                raise_signal();
            }
            check_pending();
        }
        else {
            fprintf(stderr, "Unknown argument!\n");
            return 1;
        }
        fflush(NULL);
    return 0;
}

int main(int argc, char** argv) {
    if ((argc != 2 && argc!=3 )|| (argc == 3 && strcmp(argv[2], "execl")) != 0) {
        fprintf(stderr, "Wrong number of arguments!\n");
        return 1;
    }
    else if (argc==2){
        is_parent=1;
    }

    char* option=argv[1];
    if(handle_signal(1, option)) return 1;
    if (is_parent) {
        execl(argv[0], argv[0], argv[1], "execl", NULL);
    }

    return 0;
}