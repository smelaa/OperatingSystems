#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[]){
    if(argc!=2){
        fprintf(stderr, "wrong number of arguments\n");
        return -1;
    }
    char *path=argv[1];

    //usuwam sciezke do pliku i zostawiam tylko nazwe programu
    char* prog_name=argv[0];
    prog_name=strrchr(prog_name, '/');
    if (prog_name!= NULL) {
		prog_name=prog_name + 1;
	}

    printf("%s: ", prog_name);

    fflush(stdout);

    if(execl("/bin/ls", "ls", path, NULL)==-1)
    {
        fprintf(stderr, "error occured while executing ls\n");
        return -1;
    }

    return 0;
}