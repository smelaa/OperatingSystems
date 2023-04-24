#include <sys/msg.h> 
#include <sys/ipc.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>

#include "heading.h"

int server_msqid;

FILE *report;

int curr_users_cnt=0;
int clients_msqids[CLIENT_MAXNUM];

void handle_init(key_t key){
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char *time_str=asctime (timeinfo);

    int user_msqid=msgget(key, 0);

    fprintf(report, "\n[NEW REQUEST | INIT] \t%s", time_str);
    printf("\n[NEW REQUEST | INIT] \t%s", time_str);

    struct msgbuf *snd_buf=malloc(sizeof(msgbuf));
    snd_buf->mtype=C_INIT;
    snd_buf->sender_id=-1;
    if (curr_users_cnt>=CLIENT_MAXNUM){
        msgsnd(user_msqid, snd_buf, sizeof(msgbuf), 0);
        printf("Cannot register new user. The limit of users has been reached.\n");
        fprintf(report, "Cannot register new client. The limit of users has been reached.\n");
    } else{
        for(int i=0;i<CLIENT_MAXNUM;i++){
            if (clients_msqids[i]==-1){
                snd_buf->sender_id=i;
                curr_users_cnt+=1;
                clients_msqids[i]=user_msqid;
                msgsnd(user_msqid, snd_buf, sizeof(msgbuf), 0);
                printf("Registration successful. Given ID: %d\n", i);
                fprintf(report, "Registration successful. Given ID: %d\n", i);
                break;
            }
        }
    }
    free(snd_buf);
}

void handle_stop(int sender_id){
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char *time_str=asctime (timeinfo);

    fprintf(report, "\n[NEW REQUEST | STOP | ID%d] \t%s", sender_id, time_str);
    printf("\n[NEW REQUEST | STOP | ID%d] \t%s", sender_id, time_str);

    clients_msqids[sender_id]=-1;
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

    struct msgbuf *snd_buf=malloc(sizeof(msgbuf));
    snd_buf->mtype=C_LIST;
    sprintf(snd_buf->mtext, "List of registered clients' IDs:");

    char tmp[MSG_MAXLEN];
    for(int i=0;i<CLIENT_MAXNUM;i++){
        if(clients_msqids[i]!=-1){
            printf(" %d;", i);
            sprintf(tmp, " %d;", i);
            strcat(snd_buf->mtext, tmp);
        }
    }
    msgsnd(clients_msqids[sender_id], snd_buf, sizeof(msgbuf), 0);
    free(snd_buf);
    printf("\nAll %d clients listed.\n", curr_users_cnt);
}

void handle_2one(int sender_id, int receiver_id, char* msg){
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char *time_str=asctime (timeinfo);

    fprintf(report, "\n[NEW REQUEST | MSG TO ID%d | ID%d] \t%s", receiver_id, sender_id, time_str);
    printf("\n[NEW REQUEST | MSG TO ID%d | ID%d] \t%s", receiver_id, sender_id, time_str);

    fprintf(report, "MSG: %s\n", msg);
    printf("MSG: %s\n", msg);

    if (receiver_id>=CLIENT_MAXNUM || clients_msqids[receiver_id]==-1){
        struct msgbuf *snd_buf=malloc(sizeof(msgbuf));
        snd_buf->mtype=C_MSGLOG;
        sprintf(snd_buf->mtext, "Message not sent. No user ID%d registered.\n", receiver_id);
        msgsnd(clients_msqids[sender_id], snd_buf, sizeof(msgbuf), 0);
        free(snd_buf);

        printf("Message not sent. No user ID%d registered.\n", receiver_id);
    }
    else{
        struct msgbuf *snd_buf=malloc(sizeof(msgbuf));
        snd_buf->mtype=C_MSG;
        snd_buf->sender_id=sender_id;
        sprintf(snd_buf->mtext, "%s", msg);
        msgsnd(clients_msqids[receiver_id], snd_buf, sizeof(msgbuf), 0);
        free(snd_buf);

        snd_buf=malloc(sizeof(msgbuf));
        snd_buf->mtype=C_MSGLOG;
        sprintf(snd_buf->mtext, "Message sent.\n");
        msgsnd(clients_msqids[sender_id], snd_buf, sizeof(msgbuf), 0);
        free(snd_buf);

        printf("Message sent.\n");
    }
}

void handle_2all(int sender_id, char* msg){
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char *time_str=asctime (timeinfo);

    fprintf(report, "\n[NEW REQUEST | MSG TO ALL | ID%d] \t%s", sender_id, time_str);
    printf("\n[NEW REQUEST | MSG TO ALL | ID%d] \t%s", sender_id, time_str);

    fprintf(report, "MSG: %s\n", msg);
    printf("MSG: %s\n", msg);

    struct msgbuf *snd_buf=malloc(sizeof(msgbuf));
    snd_buf->mtype=C_MSG;
    snd_buf->sender_id=sender_id;
    sprintf(snd_buf->mtext, "%s", msg);
    for (int i=0;i<CLIENT_MAXNUM;i++){
        if (clients_msqids[i]!=-1){
            msgsnd(clients_msqids[i], snd_buf, sizeof(msgbuf), 0);
        }
    }
    free(snd_buf);

    snd_buf=malloc(sizeof(msgbuf));
    snd_buf->mtype=C_MSGLOG;
    sprintf(snd_buf->mtext, "Message sent.\n");
    msgsnd(clients_msqids[sender_id], snd_buf, sizeof(msgbuf), 0);
    free(snd_buf);

    printf("Message sent.\n");
}

void exit_handler(void){
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char *time_str=asctime (timeinfo);

    fprintf(report, "\n***LEAVING*** \t%s", time_str);
    printf("\n***LEAVING*** \t%s", time_str);

    struct msgbuf *snd_buf=malloc(sizeof(msgbuf));
    snd_buf->mtype=C_STOP;
    
    for(int i=0;i<CLIENT_MAXNUM;i++){  
        if (clients_msqids[i]!=-1){
            msgsnd(clients_msqids[i], snd_buf, sizeof(msgbuf), 0);
        }
    }

    struct msgbuf *rec_buf=malloc(sizeof(msgbuf));
    while(curr_users_cnt!=0){
        msgrcv(server_msqid,rec_buf,sizeof(msgbuf), STOP,0);
        handle_stop(rec_buf->sender_id);
    }

    msgctl(server_msqid, IPC_RMID, NULL);

    fclose(report);

    free(snd_buf);
    free(rec_buf);

    fprintf(report, "\n***LEFT SUCCESSFULLY***\n");
    printf("\n***LEFT SUCCESSFULLY***\n");
}

void sigint_handler(int signum){
    exit(0);
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

int main(){
    key_t key = ftok(getenv("HOME"), SERVER_ID); 
    server_msqid = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    if (server_msqid < 0) {
        perror("Error while creating message queue.\n");
        exit(1);
    }

    report=fopen(SERVER_FILE,"w");

    atexit(exit_handler);

    struct sigaction act;
    act.sa_handler=sigint_handler;
    sigaction(SIGINT, &act,NULL);

    for(int i=0;i<CLIENT_MAXNUM;i++){
        clients_msqids[i]=-1;
    }

    print_success();

    while(1){
        fflush(stdout);
        struct msgbuf *rec_buf=malloc(sizeof(msgbuf));
        msgrcv(server_msqid,rec_buf,sizeof(msgbuf),-6,0);
        switch (rec_buf->mtype)
        {
        case INIT:
            handle_init(rec_buf->key);
            break;
        case STOP:
            handle_stop(rec_buf->sender_id);
            break;
        case LIST:
            handle_list(rec_buf->sender_id);
            break;
        case TOONE:
            handle_2one(rec_buf->sender_id, rec_buf->receiver_id, rec_buf->mtext);
            break;
        case TOALL:
            handle_2all(rec_buf->sender_id, rec_buf->mtext);
            break;
        default:
            print_unknown_req();
        }
        free(rec_buf);
    }


}