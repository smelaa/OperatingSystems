#define SERVER_ID 1
#define MSG_MAXLEN 300
#define CLIENT_MAXNUM 20
#define SERVER_FILE "./server_raport.txt"

typedef enum server_mtype {
	TOALL = 1,
	TOONE = 2,
    INIT = 3,
    LIST = 4,
	STOP = 5
} server_mtype;

typedef enum client_mtype {
	C_INIT = 1,
	C_MSG = 2,
    C_LIST = 3,
    C_MSGLOG=4,
	C_STOP = 5
} client_mtype;

typedef struct msgbuf {
    long mtype; 
    char mtext[MSG_MAXLEN];
    int sender_id;
    int receiver_id;   
    key_t key;
} msgbuf;
