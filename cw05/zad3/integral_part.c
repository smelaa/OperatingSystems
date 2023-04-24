#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

double f(float x) {
	return 4/(x*x+1);
}

int main(int argc, char** argv){
    double begin=strtod(argv[1],NULL);
    double end=strtod(argv[2],NULL);
    double dx=strtod(argv[3],NULL);

    double curr=dx*ceil(begin/dx);
	double sum = 0.0;

	while (curr< end) {
		sum += f(curr) * dx;
		curr += dx;
	}

	char *buffer=calloc(sizeof(char), BUFFER_SIZE);
    snprintf(buffer, BUFFER_SIZE, "%f\t", sum);
	int fifo = open("/tmp/int_fifo", O_WRONLY);
	write(fifo, buffer, BUFFER_SIZE);
	free(buffer);
	close(fifo);
    exit(EXIT_SUCCESS);
}