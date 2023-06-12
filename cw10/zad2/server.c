#include "heading.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int epoll_fd;
typedef struct sockaddr* sa;
client clients[CLIENT_MAXNUM]; 
int clients_cnt;

void delete_client(client* client) {
    printf("Removing client %s\n", client->nick);
    message msg = { .type = STOP };
    sendto(client->socket_fd, &msg, sizeof (msg), 0, (sa) &client->addr, client->addrlen);
    memset(&client->addr, 0, sizeof (client->addr));
    client->state = EMPTY;
    client->nick[0] = '\0';
    client->socket_fd = 0;
    clients_cnt--;
}

void send_msg(client* client, msgtype type, char text[MSG_MAXLEN]) {
    message msg;
    msg.type = type;
    memcpy(&msg.text, text, MSG_MAXLEN*sizeof(char));
    sendto(client->socket_fd, &msg, sizeof msg, 0, (sa) &client->addr, client->addrlen);
}

void on_client_message(client* client, message* msg_ptr) {
    message msg = *msg_ptr;

    printf("Client %s ", client->nick);
    if (msg.type == PING) {
        pthread_mutex_lock(&mutex);
        printf("is responding\n");
        client->responding = true;
        pthread_mutex_unlock(&mutex);
    } else if (msg.type == STOP) {
        printf("stopping\n");
        pthread_mutex_lock(&mutex);
        delete_client(client);
        pthread_mutex_unlock(&mutex);
    } else if (msg.type == TOALL) {
        printf("sending msg to all\n");
        char message[MSG_MAXLEN] = "";
        strcat(message, client->nick);
        strcat(message, ": ");
        strcat(message, msg.text);

        for (int i = 0; i < CLIENT_MAXNUM; i++) {
        if (clients[i].state != EMPTY)
            send_msg(clients+i, GET, message);
        }
    } else if (msg.type == TOONE) {
        printf("sending msg to %s\n", msg.other_nick);
        char message[MSG_MAXLEN] = "";
        strcat(message, client->nick);
        strcat(message, ": ");
        strcat(message, msg.text);

        for (int i = 0; i < CLIENT_MAXNUM; i++) {
        if (clients[i].state != EMPTY) {
            if (strcmp(clients[i].nick, msg.other_nick) == 0) {
            send_msg(clients+i, GET, message);
            }
        }
        }
    } else if (msg.type == LIST) {
        printf("listing\n");
        for (int i = 0; i < CLIENT_MAXNUM; i++) {
        if (clients[i].state != EMPTY)
            send_msg(client, GET, clients[i].nick);
        }
    
    } 
}

void init_socket(int socket, void* addr, int addr_size) {
    bind(socket, (struct sockaddr*) addr, addr_size);
    struct epoll_event event = { .events = EPOLLIN | EPOLLPRI, .data = { .fd = socket } 
    };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket, &event);
}

void new_client(union addr* addr, socklen_t addrlen, int socket_fd, char* nick) {
    pthread_mutex_lock(&mutex);
    int empty_i = -1;

    if (clients_cnt>=CLIENT_MAXNUM) {

        pthread_mutex_unlock(&mutex);
        printf("Server is full\n");
        message msg = { .type = FULL_SERVER };
        sendto(socket_fd, &msg, sizeof msg, 0, (sa) addr, addrlen);
        return;
    }
    for (int i = 0; i < CLIENT_MAXNUM; i++) {
        if (clients[i].state == EMPTY) empty_i = i;
        else if (strncmp(nick, clients[i].nick, sizeof clients->nick) == 0) {
        pthread_mutex_unlock(&mutex);
        message msg = {.type = NICK_TAKEN };
        printf("Nick %s is already taken\n", nick);
        sendto(socket_fd, &msg, sizeof msg, 0, (sa) addr, addrlen);
        return;
        }
    }
    if (empty_i == -1) {
        pthread_mutex_unlock(&mutex);
        printf("Cannot register new client. Server full?\n");
        message msg = { .type = FULL_SERVER };
        sendto(socket_fd, &msg, sizeof msg, 0, (sa) addr, addrlen);
        return;
    }
    clients_cnt++;
    printf("New client %s\n", nick);
    client* client = &clients[empty_i];
    memcpy(&client->addr, addr, addrlen);
    client->addrlen = addrlen;
    client->state = WAITING;
    client->responding = true;
    client->socket_fd = socket_fd;
    memset(client->nick, 0, sizeof client->nick);
    strncpy(client->nick, nick, sizeof client->nick - 1);

    pthread_mutex_unlock(&mutex);
}


void* ping(void* _) {
    const static message msg = { .type = PING };
    while (true) {
        sleep(WAITING_TIME);
        pthread_mutex_lock(&mutex);
        printf("PINGING CLIENTS\n");
        for (int i = 0; i < CLIENT_MAXNUM; i++) {
        if (clients[i].state != EMPTY) {
            if (clients[i].responding) {
            clients[i].responding = false;
            sendto(clients[i].socket_fd, &msg, sizeof msg, 0, (sa) &clients[i].addr, clients[i].addrlen);
            }
            else delete_client(&clients[i]);
        }
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void handle_sigint(int _) {
    for (int i = 0; i < CLIENT_MAXNUM; i++) {
        if (clients[i].state != EMPTY)
            delete_client(clients+i);
        }
    exit(0);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Wrong number of arguments. Type: *port* *path*\n");
        exit(0);
    }
    int port = atoi(argv[1]);
    char* socket_path = argv[2];
    clients_cnt=0;
    signal(SIGINT, handle_sigint);

    epoll_fd = epoll_create1(0);
    struct sockaddr_un local_addr = { .sun_family = AF_UNIX };
    strncpy(local_addr.sun_path, socket_path, sizeof local_addr.sun_path);

    struct sockaddr_in web_addr = {
        .sin_family = AF_INET, .sin_port = htons(port),
        .sin_addr = { .s_addr = htonl(INADDR_ANY) },
    };

    unlink(socket_path);
    int local_socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    init_socket(local_socket_fd, &local_addr, sizeof local_addr);
    int web_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    init_socket(web_socket_fd, &web_addr, sizeof web_addr);

    pthread_t pinging_thread;
    pthread_create(&pinging_thread, NULL, ping, NULL);

    struct epoll_event events[1028];
    while(true){
        int nread = epoll_wait(epoll_fd, events, 1028, -1);
        for(int i=0;i<nread;i++) {
            int socket_fd = events[i].data.fd;
            message msg;
            union addr addr;
            socklen_t addrlen = sizeof addr;
            recvfrom(socket_fd, &msg, sizeof msg, 0, (sa) &addr, &addrlen);
            if (msg.type == CONNECT) {
                new_client(&addr, addrlen, socket_fd, msg.nick);
            } else {
                int j;
                for (j=0;j<CLIENT_MAXNUM;j++){
                    if(memcmp(&clients[j].addr, &addr, addrlen) == 0) break;
                } 
                on_client_message(&clients[j], &msg);
            }
        }
    }
}