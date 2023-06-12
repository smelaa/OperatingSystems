#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <pthread.h>

#define MSG_MAXLEN 1024
#define CLIENT_MAXNUM 10
#define WAITING_TIME 30

typedef enum {
    PING,
    NICK_TAKEN,
    FULL_SERVER,
    GET,
    INIT,
    LIST,
    TOONE,
    TOALL,
    STOP,
  } msgtype;

typedef struct message {
  msgtype type;
  char text[MSG_MAXLEN]; 
  char other_nick[MSG_MAXLEN];
} message;

typedef enum event_type {
  SOCKET,
  CLIENT
} event_type;

typedef struct event_data {
  event_type type;
  union {
    int socket;
    struct client* client;
  } payload;
} event_data;

typedef struct client {
  int fd;
  enum client_state {
    EMPTY = 0,
    C_INIT,
    READY
  } state;
  char nick[MSG_MAXLEN];
  char symbol;
  struct game_state* game_state;
  bool responding;
} client;