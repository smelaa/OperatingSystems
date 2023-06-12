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
  CONNECT,
  LIST,
  TOONE,
  TOALL,
  STOP,
} msgtype;


typedef struct {
  msgtype type;
  char text[MSG_MAXLEN];
  char nick[MSG_MAXLEN];
  char other_nick[MSG_MAXLEN];
} message;

union addr {
  struct sockaddr_un uni;
  struct sockaddr_in web;
};

typedef struct client {
  union addr addr;
  int socket_fd, addrlen;
  enum client_state { EMPTY = 0, WAITING, PLAYING } state;
  struct client* peer;
  char nick[MSG_MAXLEN];
  bool responding;
} client;

typedef struct event_data {
  enum event_type { SOCKET, CLIENT} type;
  union payload { client* client; int socket; } payload;
} event_data;