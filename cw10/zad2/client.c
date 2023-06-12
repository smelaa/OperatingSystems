#include "heading.h"
int socket_fd;

int connect_unix(char* path, char* user) {
    struct sockaddr_un addr, bind_addr;
    memset(&addr, 0, sizeof(addr));
    bind_addr.sun_family = AF_UNIX;
    addr.sun_family = AF_UNIX;
    snprintf(bind_addr.sun_path, sizeof bind_addr.sun_path, "/tmp/%ld%s", time(NULL), user);
    strncpy(addr.sun_path, path, sizeof addr.sun_path);
    int socketfd = socket(AF_UNIX, SOCK_DGRAM, 0);	
    bind(socketfd, (void*) &bind_addr, sizeof addr);
    connect(socketfd, (struct sockaddr*) &addr, sizeof addr);

    return socketfd;
}

int connect_web(char* ipv4, int port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ipv4, &addr.sin_addr) <= 0) {
        printf("Invalid address\n");
        exit(0);
    }
    int socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    connect(socketfd, (struct sockaddr*) &addr, sizeof addr);
    
    return socketfd;
}

void handle_sigint(int _) {
    message msg = { .type = STOP };
    sendto(socket_fd, &msg, sizeof msg, 0, NULL, sizeof(struct sockaddr_in));
    exit(0);
}

int main(int argc, char** argv) {
    if (strcmp(argv[2], "web") == 0 && argc == 5)
        socket_fd = connect_web(argv[3], atoi(argv[4]));
    else if (strcmp(argv[2], "unix") == 0 && argc == 4)
        socket_fd = connect_unix(argv[3], argv[1]);
    else {
        printf("Wrong arguments. Type: *nick* *web|unix* *ip+port|path*\n\n");
        exit(0);
    }
    char* nick = argv[1];

    signal(SIGINT, handle_sigint);
    message msg = { .type = CONNECT };
    strncpy(msg.nick, nick, sizeof msg.nick);
    send(socket_fd, &msg, sizeof msg, 0);
    
    int epoll_fd = epoll_create1(0);
    struct epoll_event stdin_event = { 
        .events = EPOLLIN | EPOLLPRI,
        .data = { .fd = STDIN_FILENO }
    };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &stdin_event);

    struct epoll_event socket_event = { 
        .events = EPOLLIN | EPOLLPRI,
        .data = { .fd = socket_fd }
    };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &socket_event);

    struct epoll_event events[2];
    while(true) {
        int nread = epoll_wait(epoll_fd, events, 2, 1);
        for(int i=0;i<nread;i++) {
        if (events[i].data.fd == STDIN_FILENO) {
            char buffer[512] = {};

            int size = read(STDIN_FILENO, &buffer, 512);
            buffer[size] = 0;

            char separator[] = " \t\n";
            char *token;
            token = strtok( buffer, separator);

            msgtype type = -1;
            char other_nick[MSG_MAXLEN] = {};
            char text[MSG_MAXLEN] = {};

            if (token == NULL) continue;
            if (strcmp(token, "LIST") == 0) {
                type = LIST;
            }
            else if (strcmp(token, "2ALL") == 0) {
                type = TOALL;
                
                token = strtok(NULL, "\n");
                memcpy(text, token, strlen(token) * sizeof(char));
                text[strlen(token)] = 0;
            }
            else if (strcmp(token, "2ONE") == 0) {
                type = TOONE;
                
                token = strtok(NULL, separator);
                memcpy(nick, token, strlen(token) * sizeof(char));
                nick[strlen(token)] = 0;
                
                token = strtok(NULL, "\n");
                memcpy(text, token, strlen(token) * sizeof(char));
                text[strlen(token)] = 0;
            }
            else if (strcmp(token, "STOP") == 0) {
                type = STOP;
            }
            else {
                printf("Invalid command");
                fflush(stdout);
                type = -1;
            }

            message msg;
            msg.type = type;
            memcpy(&msg.other_nick, other_nick, strlen(other_nick)+1);
            memcpy(&msg.text, text, strlen(text)+1);

            sendto(socket_fd, &msg, sizeof msg, 0, NULL, sizeof(struct sockaddr_in));

        } else { 
            message msg;
            recvfrom(socket_fd, &msg, sizeof msg, 0, NULL, NULL);

            if (msg.type == NICK_TAKEN) {
            printf("This nick is already taken\n");
            close(socket_fd);
            exit(0);
            } else if (msg.type == FULL_SERVER) {
            printf("Server is full\n");
            close(socket_fd);
            exit(0);
            } else if (events[i].events & EPOLLHUP) {
            printf("Server disconnected\n");
            exit(0);
            } else if (msg.type == PING) {
            sendto(socket_fd, &msg, sizeof msg, 0, NULL, sizeof(struct sockaddr_in));
            } else if (msg.type == STOP) {
            printf("Stopping\n");
            fflush(stdout);
            close(socket_fd);
            exit(0);
            } else if (msg.type == GET) {
            printf("%s\n",msg.text);
            }
            fflush(stdout);
        }


        }
    }
}