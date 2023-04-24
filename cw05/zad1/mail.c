#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];

int main(int argc, char** argv) {
	if (argc < 2 || argc == 3 || argc>4) {
		fprintf(stderr, "Wrong number of arguments!\n");
		return -1;
	}
    if (argc == 2){
        char *cmd;
        if(strcmp(argv[1], "data") == 0){
            cmd="mail -H";
        } else if (strcmp(argv[1], "nadawca") == 0){
            cmd="mail -H | sort -k3";
        } else{
            fprintf(stderr, "Wrong arguments!\n");
            return -1;
        }
        FILE* input = popen(cmd, "r");
        int size = fread(buffer, sizeof(char), BUFFER_SIZE, input);
        buffer[size]='\0';
        printf("%s\n", buffer);
		pclose(input);
    }  else{
        char cmd[]="mail -s ";
        strcat(cmd, argv[2]);
        strcat(cmd, " ");
        strcat(cmd, argv[1]);
        FILE* input = popen(cmd, "w");
		fwrite(argv[3], sizeof(char), strlen(argv[3]), input);
		pclose(input);
    }
}