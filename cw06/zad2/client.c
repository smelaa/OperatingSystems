#include <mqueue.h>
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

mqd_t server_q;
mqd_t user_q;
int idx;
char name[NAME_SIZE];

void sigint_handler(int signum){
    exit(0);
}

void stop(void){
    struct msgbuf msg;
    printf("\n***LEAVING***\n");
    msg.mtype=STOP;
    msg.sender_id=idx;
    mq_send(server_q, (char *) &msg, sizeof(msgbuf), 0);
    mq_close(server_q);
    mq_close(user_q);
    mq_unlink(name);
    printf("***LEFT SUCCESSFULLY***\n");
}

void list(void){
    struct msgbuf msg;
    msg.mtype=LIST;
    msg.sender_id=idx;
    mq_send(server_q, (char *) &msg, sizeof(msgbuf), 0);
    printf("List request sent.\n");
    
    mq_receive(user_q, (char *) &msg, sizeof(msgbuf), NULL);
    printf("%s\n",msg.mtext);
}

void send_msg_2one(int receiver_id, char *message){
    struct msgbuf msg;
    msg.mtype=TOONE;
    msg.sender_id=idx;
    msg.receiver_id=receiver_id;
    strcpy(msg.mtext, message);
    mq_send(server_q, (char *) &msg, sizeof(msgbuf), 0);
    printf("Sending message to ID%d...\n", receiver_id);
    mq_receive(user_q, (char *) &msg, sizeof(msgbuf), NULL);
    printf("%s",msg.mtext);
}

void send_msg_2all(char *message){
    struct msgbuf msg;
    msg.mtype=TOALL;
    msg.sender_id=idx;
    strcpy(msg.mtext, message);
    mq_send(server_q, (char *) &msg, sizeof(msgbuf), 0);
    printf("Sending message to all..\n");
    mq_receive(user_q, (char *) &msg, sizeof(msgbuf), NULL);
    printf("%s",msg.mtext);
}

void random_name(void) {
	name[0] = '/';

	for (int i = 1; i < NAME_SIZE; i++) {
		name[i] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[random () % 26];
	}
}

void handle_server_message(void){
    struct timespec my_time;
	clock_gettime(CLOCK_REALTIME, &my_time);
	my_time.tv_sec += 0.1;

    struct msgbuf msg;
    while(mq_timedreceive(user_q, (char *) &msg, sizeof(msgbuf), NULL, &my_time) != -1){
        if(msg.mtype==C_STOP){
            printf("\n[SERVER LEAVING]");
            exit(0);
        }  
        else if(msg.mtype==C_MSG){
            printf("\n[MSG] FROM ID%d: %s\n", msg.sender_id, msg.mtext);
        }
        else{
            printf("\n[UNEXPECTED REQUEST FROM SERVER]\n");
        }
    }
}

int main(){
    srand(time(NULL));

    server_q=mq_open(SERVER_ID, O_RDWR);
    if (server_q < 0) {
        perror("Error while opening server's message queue.\n");
        exit(1);
    }

    random_name();
    mq_unlink(name);
    struct mq_attr attr;
	attr.mq_maxmsg = CLIENT_MAXNUM;
	attr.mq_msgsize = sizeof(msgbuf);
	user_q=mq_open(name, O_RDWR | O_CREAT | O_EXCL , 0666, &attr);

    if (user_q < 0) {
        perror("Error while creating message queue.\n");
        exit(1);
    }

    atexit(stop);

    struct sigaction act;
    act.sa_handler=sigint_handler;
    sigaction(SIGINT, &act,NULL);
    
    struct msgbuf msg;
    msg.mtype=INIT;
    strcpy(msg.name, name);
    if(mq_send(server_q, (char *) &msg, sizeof(msgbuf), 0)==-1){
        perror("Error while sending registration request to server.\n");
        exit(1);
    } 

    printf("Registration request sent to server.\n");
    
    mq_receive(user_q, (char *) &msg, sizeof(msgbuf), NULL);
    
    if(msg.sender_id==-1){
        printf("Registration request denied.\n");
        exit(0);
    }
    else{
        idx=msg.sender_id;
        printf("Registration sucessful. Client ID: %d\n", idx);
    }

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