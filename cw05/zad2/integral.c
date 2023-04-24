#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <math.h>

#define BUFFER_SIZE 1024

double f(float x) {
	return 4/(x*x+1);
}

double integrate_part(double begin, double end, double dx) {
    double curr=dx*ceil(begin/dx);
	double sum = 0.0;

	while (curr< end) {
		sum += f(curr) * dx;
		curr += dx;
	}
	return sum;
}

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

    int fds[n];

    clock_t start =clock();

    for (int i=0;i<n;i++){
        int fd[2];
        pipe(fd);
        if(fork()==0){
            close(fd[0]);
            double result=integrate_part(i*1.0/n, (i+1)*1.0/n, dx);
            char *buffer=calloc(sizeof(char), BUFFER_SIZE);
            int size = snprintf(buffer, BUFFER_SIZE, "%f", result);
            write(fd[1], buffer, size);
            free(buffer);
            return 0;
        }
        else{
            close(fd[1]);
            fds[i]=fd[0];
        }
    }


    double int_res=0.0;

    wait(NULL);
    
    for (int i=0;i<n; i++){
        char *buffer=calloc(sizeof(char), BUFFER_SIZE);
        int size=read(fds[i], buffer, BUFFER_SIZE);
        buffer[size]='\0';
        int_res+=strtod(buffer,NULL);
        free(buffer);
    }
    clock_t end=clock();

    double time=((double) (end-start))/CLOCKS_PER_SEC;

    printf("Arguments: n = %d, dx = %.1e\nRESULT: %f\tTIME:%f\n=======================================\n", n, dx, int_res, time);

    return 0;
}