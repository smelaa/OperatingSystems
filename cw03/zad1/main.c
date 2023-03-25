#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
    if(argc!=2){
        fprintf(stderr, "wrong number of arguments\n");
        return -1;
    }
    int n;
    if(! (n=atoi(argv[1]))){
        fprintf(stderr, "argument needs to be a number\n");
        return -1;
    }

    for (int i=0;i<n;i++){
        int fpid=fork(); //the PID of the child process is returned in the parent, and 0 is returned in the child
        if(fpid==0){
            printf("%d.\t PPID: %d\t PID: %d\n", i+1, getppid(), getpid());
            return 0;
        }
        wait(NULL);
    }
    //dlaczego jesli tutaj dam wait to nie dziala?
    printf("%d\n", n);
    return 0;
}