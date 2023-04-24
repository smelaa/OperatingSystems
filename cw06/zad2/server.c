#include <mqueue.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "heading.h"

mqd_t server_q;
FILE *report;
mqd_t clients_q[CLIENT_MAXNUM];
int curr_users_cnt=0;

void handle_init(char* name){
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char *time_str=asctime (timeinfo);

    fprintf(report, "\n[NEW REQUEST | INIT] \t%s", time_str);
    printf("\n[NEW REQUEST | INIT] \t%s", time_str);

    int user_q=mq_open(name, O_RDWR);
    if (user_q < 0) {
        printf("Error while opening user's message queue.\n");
        return;
    }

    struct msgbuf msg;
    msg.mtype=C_INIT;
    msg.sender_id=-1;
    if (curr_users_cnt>=CLIENT_MAXNUM){
        printf("Cannot register new user. The limit of users has been reached.\n");
        fprintf(report, "Cannot register new client. The limit of users has been reached.\n");
    } else{
        for(int i=0;i<CLIENT_MAXNUM;i++){
            if (clients_q[i]==-1){
                msg.sender_id=i;
                curr_users_cnt++;
                clients_q[i]=user_q;
                
                printf("Registration successful. Given ID: %d\n", i);
                fprintf(report, "Registration successful. Given ID: %d\n", i);
                break;
            }
        }
    }
    mq_send(user_q, (char *) &msg, sizeof(msgbuf), 0);
}

void handle_stop(int sender_id){
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char *time_str=asctime (timeinfo);

    fprintf(report, "\n[NEW REQUEST | STOP | ID%d] \t%s", sender_id, time_str);
    printf("\n[NEW REQUEST | STOP | ID%d] \t%s", sender_id, time_str);

    mq_close(clients_q[sender_id]);
    clients_q[sender_id]=-1;
    curr_users_cnt--;

    fprintf(report, "User ID%d left the chat.\n", sender_id);
    printf("User ID%d left the chat.\n", sender_id);
}

void handle_list(int sender_id){
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char *time_str=asctime (timeinfo);

    fprintf(report, "\n[NEW REQUEST | LIST | ID%d] \t%s", sender_id, time_str);
    printf("\n[NEW REQUEST | LIST | ID%d] \t%s", sender_id, time_str);
    
    printf("List of registered clients' IDs:");

    msgbuf msg;
    msg.mtype=C_LIST;
    sprintf(msg.mtext, "List of registered clients' IDs:");

    char tmp[MSG_MAXLEN];
    for(int i=0;i<CLIENT_MAXNUM;i++){
        if(clients_q[i]!=-1){
            printf(" %d;", i);
            sprintf(tmp, " %d;", i);
            strcat(msg.mtext, tmp);
        }
    }
    mq_send(clients_q[sender_id], (char *) &msg, sizeof(msgbuf), 0);
    printf("\nAll %d clients listed.\n", curr_users_cnt);
}

void handle_2one(int sender_id, int receiver_id, char* message){
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char *time_str=asctime (timeinfo);

    fprintf(report, "\n[NEW REQUEST | MSG TO ID%d | ID%d] \t%s", receiver_id, sender_id, time_str);
    printf("\n[NEW REQUEST | MSG TO ID%d | ID%d] \t%s", receiver_id, sender_id, time_str);

    fprintf(report, "MSG: %s\n", message);
    printf("MSG: %s\n", message);

    if (receiver_id>=CLIENT_MAXNUM || clients_q[receiver_id]==-1){
        struct msgbuf msg;
        msg.mtype=C_MSGLOG;
        sprintf(msg.mtext, "Message not sent. No user ID%d registered.\n", receiver_id);
        mq_send(clients_q[sender_id], (char *) &msg, sizeof(msgbuf), 0);
        
        printf("Message not sent. No user ID%d registered.\n", receiver_id);
    }
    else{
        struct msgbuf msg;
        msg.mtype=C_MSG;
        msg.sender_id=sender_id;
        sprintf(msg.mtext, "%s", message);
        mq_send(clients_q[receiver_id], (char *) &msg, sizeof(msgbuf), 0);

        msg.mtype=C_MSGLOG;
        sprintf(msg.mtext, "Message sent.\n");
        mq_send(clients_q[sender_id], (char *) &msg, sizeof(msgbuf), 0);

        printf("Message sent.\n");
    }
}

void handle_2all(int sender_id, char* message){
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char *time_str=asctime (timeinfo);

    fprintf(report, "\n[NEW REQUEST | MSG TO ALL | ID%d] \t%s", sender_id, time_str);
    printf("\n[NEW REQUEST | MSG TO ALL | ID%d] \t%s", sender_id, time_str);

    fprintf(report, "MSG: %s\n", message);
    printf("MSG: %s\n", message);

    struct msgbuf msg;
    msg.mtype=C_MSG;
    msg.sender_id=sender_id;
    sprintf(msg.mtext, "%s", message);
    for (int i=0;i<CLIENT_MAXNUM;i++){
        if (clients_q[i]!=-1 && sender_id!=i){
            mq_send(clients_q[i], (char *) &msg, sizeof(msgbuf), 0);
        }
    }

    msg.mtype=C_MSGLOG;
    sprintf(msg.mtext, "Message sent.\n");
    mq_send(clients_q[sender_id], (char *) &msg, sizeof(msgbuf), 0);

    printf("Message sent.\n");
}

void print_success(void){
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char *time_str=asctime (timeinfo);

    fprintf(report, "[SERVER INIT SUCCESS] \t%s", time_str);
    printf("[SERVER INIT SUCCESS] \t%s", time_str);
}

void print_unknown_req(void){
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char *time_str=asctime (timeinfo);

    fprintf(report, "[UNKNOWN REQUEST] \t%s", time_str);
    printf("[UNKNOWN REQUEST] \t%s", time_str);
}

void exit_handler(void){
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char *time_str=asctime (timeinfo);

    fprintf(report, "\n***LEAVING*** \t%s", time_str);
    printf("\n***LEAVING*** \t%s", time_str);

    msgbuf msg;
    
    for(int i=0;i<CLIENT_MAXNUM;i++){  
        if (clients_q[i]!=-1){
            msg.mtype = C_STOP;
			mq_send(clients_q[i], (char *) &msg, sizeof(msgbuf), 0);
            handle_stop(i);
        }
    }

    mq_close(server_q);
    mq_unlink(SERVER_ID);

    fclose(report);

    fprintf(report, "\n***LEFT SUCCESSFULLY***\n");
    printf("\n***LEFT SUCCESSFULLY***\n");
}

void sigint_handler(int signum){
    exit(0);
}

int main(){
    mq_unlink(SERVER_ID);
    struct mq_attr attr;
	attr.mq_maxmsg = CLIENT_MAXNUM;
	attr.mq_msgsize = sizeof(struct msgbuf);
	server_q=mq_open(SERVER_ID, O_RDWR | O_CREAT | O_EXCL , 0666, &attr);

    if (server_q < 0) {
        perror("Error while creating message queue.\n");
        exit(1);
    }

    for(int i=0;i<CLIENT_MAXNUM;i++){
        clients_q[i]=-1;
    }

    report=fopen(SERVER_FILE,"w");

    atexit(exit_handler);

    struct sigaction act;
    act.sa_handler=sigint_handler;
    sigaction(SIGINT, &act,NULL);

    print_success();

    msgbuf msg;

    while(1){
        mq_receive(server_q, (char *) &msg, sizeof(msgbuf), NULL);
        switch (msg.mtype){
            case INIT:
                handle_init(msg.name);
                break;
            case STOP:
                handle_stop(msg.sender_id);
                break;
            case LIST:
                handle_list(msg.sender_id);
                break;
            case TOONE:
                handle_2one(msg.sender_id, msg.receiver_id, msg.mtext);
                break;
            case TOALL:
                handle_2all(msg.sender_id, msg.mtext);
                break;
            default:
                print_unknown_req();
        }
    }


}