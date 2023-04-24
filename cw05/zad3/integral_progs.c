#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];

int main(int argc, char** argv){
    if (argc !=3){
        fprintf(stderr, "Wrong number of arfuments!\n");
        return -1;
    }    
    
    double dx = strtod(argv[1], NULL);
	int n = atoi(argv[2]);
	if (n*dx > 1 || dx<0 || n<0) {
		fprintf(stderr, "Wrong arguments!\n");
        return -1;
	}

    clock_t start =clock();

    mkfifo("/tmp/int_fifo", 0666);
    
    for (int i=0;i<n;i++){
        char begin[BUFFER_SIZE]="";
        char end[BUFFER_SIZE]="";
        if(fork()==0){
            snprintf(begin, BUFFER_SIZE, "%lf", i*1.0/n);
            snprintf(end, BUFFER_SIZE, "%lf", (i+1)*1.0/n);
            execl("./integral_part", "integral_part", begin, end, argv[1], NULL);
            return 0;
        } 
    }

    double int_res=0.0;

    int fifo=open("/tmp/int_fifo", O_RDONLY);
    
    int cnt=0;

    while(cnt<n){
        wait(NULL);
        int size = read(fifo, buffer, BUFFER_SIZE);
		buffer[size] = '\0';

        char *part = strtok(buffer, "\t");
        while (part!= NULL){
            double p=strtod(part,NULL);
            int_res+=p;
            cnt++;
            part = strtok(NULL, "\t");
        }
    }

    close(fifo);

    clock_t end=clock();

    double time=((double) (end-start))/CLOCKS_PER_SEC;

    printf("Arguments: n = %d, dx = %.1e\nRESULT: %f\tTIME:%f\n=======================================\n", n, dx, int_res, time);

    return 0;
}
