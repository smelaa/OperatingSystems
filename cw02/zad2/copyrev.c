#include <stdio.h>
#include <string.h>
#include <time.h>

int copy_1char(char* to_copy, char* copied){
    FILE * src_f=fopen ( to_copy, "r");
    if(!src_f){
        fprintf(stderr, "copy_1char: could not open file to copy\n");
        return -1;
    }
    FILE * res_f=fopen ( copied, "w");
    if(!res_f){
        fprintf(stderr, "copy_1char: could not open output file\n");
        return -1;
    }
    fseek(src_f, 0, SEEK_END);
    char buff;
    do {
        fseek(src_f, -1, SEEK_CUR);
        fread(&buff, sizeof(char),1,src_f);
        fwrite(&buff, sizeof(char),1,res_f);
        fseek(src_f, -1, SEEK_CUR);
    }
    while(ftell(src_f) != 0);
    fclose(src_f);
    fclose(res_f);
    return 0;
}

int copy_1024char(char* to_copy, char* copied){
    FILE * src_f=fopen ( to_copy, "r");
    if(!src_f){
        fprintf(stderr, "copy_1024char: could not open file to copy\n");
        return -1;
    }
    FILE * res_f=fopen ( copied, "w");
    if(!res_f){
        fprintf(stderr, "copy_1024char: could not open output file\n");
        return -1;
    }
    fseek(src_f, 0, SEEK_END);
    char buff[1024];
    char revbuff[1024];
    do {
        if(fseek(src_f, -1024, SEEK_CUR)!=0){
            fseek(src_f, 0, SEEK_SET);
        };
        size_t count = fread(&buff, sizeof(char),1024,src_f);
        for(size_t j = 0; j < count; j++) {
			revbuff[count - 1 - j] = buff[j];
		}
        fwrite(&revbuff, sizeof(char),count,res_f);
        if(fseek(src_f, -2048, SEEK_CUR)!=0){
            fseek(src_f, 0, SEEK_SET);
        };
    }
    while(ftell(src_f) != 0);
    fclose(src_f);
    fclose(res_f);
    return 0;
    
}

int main(int argc, char* argv[]){
    if(argc!=3){
        fprintf(stderr, "Wrong number of arguments\n");
        return -1;
    }

    char *to_copy=argv[1];
    char *copied=argv[2];

    clock_t start, end;
    char* report_file = "pomiar_zad_2.txt";
	FILE* report = fopen(report_file, "wa");

    start=clock();
    int res=copy_1char(to_copy, copied);
    end=clock();

    if(res!=0){
        fprintf(stderr, "error while executing copy_1char\n");
        fprintf(report, "error while executing copy_1char\n");
        return -1;
    }
    else{
        fprintf(report, "copying 1 char:\n execution time: %f sec\n", (double)(end - start) / CLOCKS_PER_SEC);
    }

    start=clock();
    res=copy_1024char(to_copy, copied);
    end=clock();

    if(res!=0){
        fprintf(stderr, "error while executing copy_1024char\n");
        fprintf(report, "error while executing copy_1024char\n");
        return -1;
    }
    else{
        fprintf(report, "copying 1024 char:\n execution time: %f sec\n", (double)(end - start) / CLOCKS_PER_SEC);
    }

    fclose(report);
    return 0;
}