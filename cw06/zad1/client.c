#include <sys/msg.h> 
#include <sys/ipc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include<time.h>
#include <unistd.h>
#include <sys/select.h>

#include "heading.h"

int server_msqid;
int user_msqid;
int idx;

void stop(void){
    struct msgbuf *snd_buf=malloc(sizeof(msgbuf));
    printf("\n***LEAVING***\n");
    snd_buf->mtype=STOP;
    snd_buf->sender_id=idx;
    msgsnd(server_msqid, snd_buf, sizeof(msgbuf), 0);
    msgctl(user_msqid, IPC_RMID, NULL);
    free(snd_buf);
    printf("***LEFT SUCCESSFULLY***\n");
}

void list(void){
    struct msgbuf *snd_buf=malloc(sizeof(msgbuf));
    snd_buf->mtype=LIST;
    snd_buf->sender_id=idx;
    msgsnd(server_msqid, snd_buf, sizeof(msgbuf), 0);
    printf("List request sent.\n");
    struct msgbuf *rcv_buf=malloc(sizeof(msgbuf));
    msgrcv(user_msqid,rcv_buf,sizeof(msgbuf),C_LIST,0);
    printf("%s\n",rcv_buf->mtext);
    free(rcv_buf);
    free(snd_buf);
}

void send_msg_2one(int receiver_id, char *msg){
    struct msgbuf *snd_buf=malloc(sizeof(msgbuf));
    snd_buf->mtype=TOONE;
    snd_buf->sender_id=idx;
    snd_buf->receiver_id=receiver_id;
    strcpy(snd_buf->mtext, msg);
    msgsnd(server_msqid, snd_buf, sizeof(msgbuf), 0);
    printf("Sending message to ID%d...\n", receiver_id);
    struct msgbuf *rcv_buf=malloc(sizeof(msgbuf));
    msgrcv(user_msqid,rcv_buf,sizeof(msgbuf),C_MSGLOG,0);
    printf("%s",rcv_buf->mtext);
    free(rcv_buf);
    free(snd_buf);
}

void send_msg_2all(char *msg){
    struct msgbuf *snd_buf=malloc(sizeof(msgbuf));
    snd_buf->mtype=TOALL;
    snd_buf->sender_id=idx;
    strcpy(snd_buf->mtext, msg);
    msgsnd(server_msqid, snd_buf, sizeof(msgbuf), 0);
    printf("Sending message to all..\n");
    struct msgbuf *rcv_buf=malloc(sizeof(msgbuf));
    msgrcv(user_msqid,rcv_buf,sizeof(msgbuf),C_MSGLOG,0);
    printf("%s",rcv_buf->mtext);
    free(rcv_buf);
    free(snd_buf);
}

void sigint_handler(int signum){
    exit(0);
}

void handle_server_message(void){
    struct msgbuf *rec_buf=malloc(sizeof(msgbuf));
    while(msgrcv(user_msqid, rec_buf, sizeof(msgbuf), 0, IPC_NOWAIT) >= 0){
        if(rec_buf->mtype==C_STOP){
            printf("\n[SERVER LEAVING]");
            free(rec_buf);
            exit(0);
        }  
        else if(rec_buf->mtype==C_MSG){
            printf("\n[MSG] FROM ID%d: %s\n", rec_buf->sender_id, rec_buf->mtext);
        }
        else{
            printf("\n[UNEXPECTED REQUEST FROM SERVER]\n");
        }
    }
    free(rec_buf);
}

int main(){
    srand(time(NULL));

    key_t key = ftok(getenv("HOME"), SERVER_ID); 
    server_msqid = msgget(key, 0);
    if (server_msqid < 0) {
        perror("Error while opening server's message queue.\n");
        exit(1);
    }

    key = ftok(getenv("HOME"), rand() % 255 + 1); 
    user_msqid = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    if (user_msqid<0) {
        perror("Error while creating user's message queue.\n");
        exit(1);
    }


    atexit(stop);

    struct sigaction act;
    act.sa_handler=sigint_handler;
    sigaction(SIGINT, &act,NULL);
    
    struct msgbuf *snd_buf=malloc(sizeof(msgbuf));
    snd_buf->mtype=INIT;
    snd_buf->key=key;
    msgsnd(server_msqid, snd_buf, sizeof(msgbuf), 0);
    printf("Registration request sent to server.\n");
    
    struct msgbuf *rec_buf=malloc(sizeof(msgbuf));
    msgrcv(user_msqid,rec_buf,sizeof(msgbuf),C_INIT,0);
    
    if(rec_buf->sender_id==-1){
        printf("Registration request denied.\n");
        free(rec_buf);
        free(snd_buf);
        exit(0);
    }
    else{
        idx=rec_buf->sender_id;
        printf("Registration sucessful. Client ID: %d\n", idx);
    }
    free(rec_buf);
    free(snd_buf);

    printf("\nAvailable requests:\nLIST\n2ALL\t*message*\n2ONE\t*client_id*\t*message*\nSTOP\n_____________________________\n");

    char *command;
    size_t len = 0;
    ssize_t read;

    while(1){
        printf("\n> ");
        read=getline(&command, &len, stdin);
        command[read-1]='\0';

        handle_server_message();

        if (strcmp(command, "")==0){continue;}
        char *cmd = strtok(command, " ");

        if(strcmp(cmd, "STOP")==0){exit(0);}
        else if(strcmp(cmd, "LIST")==0){list();}
        else if(strcmp(cmd, "2ONE")==0){
            int receiver_id=atoi(strtok(NULL, " "));
            cmd=strtok(NULL, "\n");
            send_msg_2one(receiver_id, cmd);
        }
        else if(strcmp(cmd, "2ALL")==0){
            cmd=strtok(NULL, "\n");
            send_msg_2all(cmd);
        }
        else{printf("Unknown request.\n");}  
    }
}