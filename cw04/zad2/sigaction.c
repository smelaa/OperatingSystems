#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
int cnt;

void siginfo_handler(int signum, siginfo_t *info, void *context) {
    printf("Signal number:\t%d\nPID:\t\t%d\n", signum, info->si_pid);
    printf("UID:\t\t%d\nSignal code:\t%d\nErrno:\t\t%d\n", info->si_uid, info->si_code,info->si_errno);
}

void nodefer_handler(int signum, siginfo_t *info, void *context) {
    printf("+ Received signal\t%d\n", cnt);
    cnt+=1;
    int cnt_copy=cnt;
    if (cnt<5){
        raise(SIGUSR1);}
    printf("- Handled signal\t%d\n", cnt_copy-1);
}

void reset_handler(int signum, siginfo_t *info, void *context) {
    printf("Resetting handler for signal %d\n", signum);
}

int main(int argc, char** argv) {
    if (argc != 1) {
        fprintf(stderr, "Wrong number of arguments!\n");
        return 1;
    }
    
    // SA_SIGINFO - procedura obslugi sygnalu przyjmuje 3 argumenty, mamy dostep do wiekszej ilosci informacji
    struct sigaction action1;
    sigemptyset(&action1.sa_mask);
    action1.sa_flags = SA_SIGINFO;
    action1.sa_sigaction = siginfo_handler;
    sigaction(SIGUSR1, &action1, NULL);
    
    printf("### SIGINFO\n");
    printf("## PARENT:\n");
    raise(SIGUSR1);
    printf("## CHILD:\n");
    if (fork() == 0) {
        raise(SIGUSR1);
        return 0;
    }
    else {
        wait(NULL);
    }
    printf("\n");


    // SA_NODEFER - umożliwienie dostarczania innych sygnałów do procesu podczas obsługi sygnału
    struct sigaction action2;
    sigemptyset(&action2.sa_mask);
    action2.sa_flags = SA_NODEFER;
    action2.sa_sigaction = nodefer_handler;
    printf("### NODEFER \n");
    sigaction(SIGUSR1, &action2, NULL);
    
    cnt=1;
    printf("## WITH FLAG SA_NODEFER\n");
    raise(SIGUSR1);

    action2.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &action2, NULL);
    
    cnt=1;    
    printf("## WITH DIFFERENT FLAG\n");
    raise(SIGUSR1);
    
    printf("\n");


    // SA_RESETHAND - handler automatycznie zreseteowany do wartosci domyslen
    struct sigaction action3;
    action3.sa_flags = SA_RESETHAND;
    action3.sa_sigaction = reset_handler;
    sigaction(SIGUSR1, &action3, NULL);

    printf("### RESETHAND\n");
    printf("Sending signal %d...\n", SIGUSR1);
    raise(SIGUSR1);
    printf("Sending signal %d again...\n", SIGUSR1);
    raise(SIGUSR1);

    return 0;
}