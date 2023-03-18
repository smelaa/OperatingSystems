#include <stdio.h>
#include <string.h>
#include <fcntl.h> //for open
#include <unistd.h> // for close
#include <time.h>

int lib(char char_from, char into_char, char* source_file, char* res_file){
    FILE * src_f=fopen ( source_file, "r");
    if(!src_f){
        fprintf(stderr, "lib: could not open source file\n");
        return -1;
    }
    FILE * res_f=fopen ( res_file, "w");
    if(!res_f){
        fprintf(stderr, "lib: could not open result file\n");
        return -1;
    }
    char curr[1];
    while(fread(curr,sizeof(char), 1,src_f)>0){
        if (*curr==char_from){
            fwrite ( &into_char, sizeof(char), 1,res_f);
        }
        else{
            fwrite ( curr, sizeof(char), 1,res_f);
        }
    }
    fclose(src_f);
    fclose(res_f);
    return 0;
}

int sys(char char_from, char into_char, char* source_file, char* res_file){
    int src_f=open(source_file, O_RDONLY);
    if(src_f==-1){
        fprintf(stderr, "sys: could not open source file\n");
        return -1;
    }
    int res_f=open(res_file, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if(src_f==-1){
        fprintf(stderr, "sys: could not open result file\n");
        return -1;
    }
    char curr[1];
    while(read(src_f,curr,1)>0){
        if (*curr==char_from){
            write ( res_f,&into_char,1);
        }
        else{
            write(res_f,curr,1);
        }
    }
    close(src_f);
    close(res_f);
    return 0;
}

int main(int argc, char* argv[]){
    if(argc!=5){
        fprintf(stderr, "Wrong number of arguments\n");
        return -1;
    }
    if (strlen(argv[1])!=1){
        fprintf(stderr, "Wrong input: 1st argument to long\n");
        return -1;
    }
    char char_from=argv[1][0];

    if (strlen(argv[2])!=1){
        fprintf(stderr, "Wrong input: 2nd argument to long\n");
        return -1;
    }
    char into_char=argv[2][0];

    char *source_file=argv[3];
    char *res_file =argv[4];

    clock_t start, end;
    char* report_file = "pomiar_zad_1.txt";
	FILE* report = fopen(report_file, "wa");

    start=clock();
    int lib_res=lib(char_from,into_char,source_file,res_file);
    end=clock();

    if(lib_res!=0){
        fprintf(stderr, "error while executing task with library functions\n");
        fprintf(report, "error while executing task with library functions\n");
        return -1;
    }
    else{
        fprintf(report, "lib functions:\n execution time: %f sec\n", (double)(end - start) / CLOCKS_PER_SEC);
    }

    start=clock();
    int sys_res=sys(char_from,into_char,source_file,res_file);
    end=clock();

    if(sys_res!=0){
        fprintf(stderr, "error while executing task with system functions\n");
        fprintf(report, "error while executing task with system functions\n");
        return -1;
    }
    else{
        fprintf(report, "sys functions:\n execution time: %f sec\n", (double)(end - start) / CLOCKS_PER_SEC);
    }

    fclose(report);
    return 0;
}