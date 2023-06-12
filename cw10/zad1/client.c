#include "heading.h"
int socket_fd;

int connect_unix(char* path) {
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    int socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
    connect(socketFd, (struct sockaddr*)&addr, sizeof(addr));

    return socketFd;
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

    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    connect(socketFd, (struct sockaddr*)&addr, sizeof(addr));

    return socketFd;
}

void handle_sigint(int tmp) {
    printf("Stopping\n");
    message msg = {.type = STOP};
    tmp = write(socket_fd, &msg, sizeof(msg));
    exit(0);
}

int main(int argc, char** argv) {
    if (strcmp(argv[2], "web") == 0 && argc == 5)
        socket_fd = connect_web(argv[3], atoi(argv[4]));
    else if (strcmp(argv[2], "unix") == 0 && argc == 4)
        socket_fd = connect_unix(argv[3]);
    else {
        printf("wrong arguments\n");
        exit(0);
    }

    signal(SIGINT, handle_sigint);
    char* nick = argv[1];
    write(socket_fd, nick, strlen(nick));

    int epoll_fd = epoll_create1(0);
    
    struct epoll_event stdin_event = {.events = EPOLLIN | EPOLLPRI, .data = {.fd = STDIN_FILENO}};
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &stdin_event); //zarejestrowanie deskryptora pliku do monitorowania
    
    struct epoll_event socket_event = {.events = EPOLLIN | EPOLLPRI , .data = {.fd = socket_fd}};
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &socket_event); //zarejestrowanie deskryptora pliku do monitorowania

    struct epoll_event events[2];
    while (1) {
        int nread = epoll_wait(epoll_fd, events, 2, 10);
        for (int i = 0; i < nread; i++) {
            if (events[i].data.fd == STDIN_FILENO) {
                char buffer[512] = {};

                int size = read(STDIN_FILENO, &buffer, 512);
                buffer[size] = 0;

                char separator[] = " \t\n";
                char* token;
                token = strtok(buffer, separator);

                msgtype message_type = -1;
                char other_nick[MSG_MAXLEN] = {};
                char text[MSG_MAXLEN] = {};

                if (token == NULL)
                    continue;
		
                if (strcmp(token, "LIST") == 0) {
                    message_type = LIST;
                }
                else if (strcmp(token, "2ALL") == 0) {
                    message_type = TOALL;
                    
                    token = strtok(NULL, "\n");
                    memcpy(text, token, strlen(token) * sizeof(char));
                                text[strlen(token)] = 0;
                }
                else if (strcmp(token, "2ONE") == 0) {
                    message_type = TOONE;
                    
                    token = strtok(NULL, separator);
                    memcpy(other_nick, token, strlen(token) * sizeof(char));
                                other_nick[strlen(token)] = 0;
                    
                    token = strtok(NULL, "\n");
                    memcpy(text, token, strlen(token) * sizeof(char));
                                text[strlen(token)] = 0;
                }
                else if (strcmp(token, "STOP") == 0) {
                    message_type = STOP;
                }
                else {
                    printf("Invalid command");
                    message_type = -1;
                }

                message msg;
                msg.type = message_type;
                memcpy(&msg.other_nick, other_nick, strlen(other_nick) + 1);
                memcpy(&msg.text, text, strlen(text) + 1);

            	write(socket_fd, &msg, sizeof(msg));
            } 
            else {
                message msg;
		        read(socket_fd, &msg, sizeof(msg));
		
                if (msg.type == NICK_TAKEN) {
                    printf("This nickname is already taken\n");
                    close(socket_fd);
                    exit(0);
                } 
                else if (msg.type == FULL_SERVER) {
                    printf("Server is full\n");
                    close(socket_fd);
                    exit(0);
                } 
                else if (events[i].events & EPOLLHUP) {
                    printf("Server disconnected\n");
                    exit(0);
                } 
                else if (msg.type == PING) {
                    write(socket_fd, &msg, sizeof(msg));
                } 
                else if (msg.type == STOP) {
                    printf("Stopping\n");
                    close(socket_fd);
                    exit(0);
                } 
                else if (msg.type == GET) {
                    printf("%s\n",msg.text);
                    fflush(stdout);
                }
            }
        }
    }
}
