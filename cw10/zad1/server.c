#include "heading.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int epoll_fd;
int clients_cnt;

client clients[CLIENT_MAXNUM];

void delete_client(client* client) {
    printf("Removing client %s\n", client->nick);
    client->state = EMPTY;
    client->nick[0] = '\0';
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->fd, NULL);
    close(client->fd);
    clients_cnt--;
}

void send_msg(client* client, msgtype type, char text[MSG_MAXLEN]) {
    message msg;
    msg.type = type;
    strncpy(msg.text, text, MSG_MAXLEN);
    write(client->fd, &msg, sizeof(msg));
}

void on_client_message(client* client) {
    if (client->state == C_INIT) {
        int nick_size = read(client->fd, client->nick, sizeof(client->nick) - 1);
        pthread_mutex_lock(&mutex);
        int i;
        for (i = 0; i < CLIENT_MAXNUM; i++) {
            if (&clients[i]!=client && strncmp(client->nick, clients[i].nick, sizeof(clients->nick)) == 0) {
                break;
            }
        } 
        if (i == CLIENT_MAXNUM) {
            client->nick[nick_size] = '\0';
            client->state = READY;
            printf("New client %s\n", client->nick);
            clients_cnt++;
        } 
        else {
            message msg = {.type = NICK_TAKEN};
            printf("Nickname %s already taken. Client not registered.\n", client->nick);
            write(client->fd, &msg, sizeof(msg));
            strcpy(client->nick, "not registered");
            delete_client(client); 
        }
        pthread_mutex_unlock(&mutex);
    } 
    else {
        message msg;
        read(client->fd, &msg, sizeof(msg));
        printf("Client %s ", client->nick);
        if (msg.type == PING){
            pthread_mutex_lock(&mutex);
            printf("%s is responding\n", client->nick);
            client->responding = true;
            pthread_mutex_unlock(&mutex);
        }
        else if (msg.type == LIST)
        {
            printf("listing\n");
            for (int i = 0; i < CLIENT_MAXNUM; i++) {
                if (clients[i].state != EMPTY) {
                    send_msg(client, GET, clients[i].nick);
                }
            }
        }
        else if (msg.type == TOONE)
        {
            printf("sending msg to %s\n", msg.other_nick);
            char message[MSG_MAXLEN] = "";
            strcat(message, client->nick);
            strcat(message, ": ");
            strcat(message, msg.text);

            for (int i = 0; i < CLIENT_MAXNUM; i++) {
                if (clients[i].state != EMPTY && strcmp(clients[i].nick, msg.other_nick) == 0) {
                    send_msg(clients + i, GET, message);
                }
            }
        }
        else if (msg.type == TOALL)
        {
            printf("sending msg to all\n");
            char message[MSG_MAXLEN] = "";
            strcat(message, client->nick);
            strcat(message, ": ");
            strcat(message, msg.text);
            for (int i = 0; i < CLIENT_MAXNUM; i++) {
                if (clients[i].state != EMPTY) {
                    send_msg(clients + i, GET, message);
                }
            }
        }
            
        else if (msg.type == STOP)
        {
            printf("stopping\n");
            pthread_mutex_lock(&mutex);
            delete_client(client);
            pthread_mutex_unlock(&mutex);
        }
    }
}

void init_socket(int socket, void* addr, int addr_size) {
    bind(socket, (struct sockaddr*)addr, addr_size);
    listen(socket, CLIENT_MAXNUM);
    struct epoll_event event = {.events = EPOLLIN | EPOLLPRI};
    event_data* event_data_ptr = malloc(sizeof(event_data));
    event_data_ptr->type = SOCKET;
    event_data_ptr->payload.socket = socket;
    event.data.ptr = event_data_ptr;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket, &event);
}

client* new_client(int client_fd) {
    pthread_mutex_lock(&mutex);
    int i;
    for (i = 0; i < CLIENT_MAXNUM; i++) {
        if (clients[i].state == EMPTY) {
            break;
        }
    }
    if (i == CLIENT_MAXNUM) {
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
    client* client = &clients[i];

    client->fd = client_fd;
    client->state = C_INIT;
    client->responding = true;
    pthread_mutex_unlock(&mutex);
    return client;
}

void* ping() {
    static message msg = {.type = PING};
    while (true) {
        sleep(WAITING_TIME);
        pthread_mutex_lock(&mutex);
        printf("PINGING CLIENTS\n");
        for (int i = 0; i < CLIENT_MAXNUM; i++) {
            if (clients[i].state != EMPTY) {
                if (clients[i].responding) {
                    clients[i].responding = false;
                    write(clients[i].fd, &msg, sizeof(msg));
                } else {
                    delete_client(&clients[i]);
                }
            }
        }
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("wrong number of arguments\n");
        exit(0);
    }
    int port = atoi(argv[1]);
    char* socket_path = argv[2];
   
    epoll_fd = epoll_create1(0);
    struct sockaddr_un local_addr = {.sun_family = AF_UNIX};
    strncpy(local_addr.sun_path, socket_path, sizeof(local_addr.sun_path));

    struct sockaddr_in web_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = {.s_addr = htonl(INADDR_ANY)},
    };

    unlink(socket_path);
    int local_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    init_socket(local_sock, &local_addr, sizeof(local_addr));

    int web_sock = socket(AF_INET, SOCK_STREAM, 0);
    init_socket(web_sock, &web_addr, sizeof(web_addr));

    pthread_t pinging_thr;
    pthread_create(&pinging_thr, NULL, ping, NULL);

    printf("Server listening on port: %d\tsocket path: '%s'\n", port, socket_path);
    clients_cnt = 0;
    struct epoll_event events[1028];
    while (true) {
        int nread = epoll_wait(epoll_fd, events, 1028, 10);
        for (int i = 0; i < nread; i++) {
            event_data* data = events[i].data.ptr;
            if (data->type == SOCKET) { 
                int client_fd = accept(data->payload.socket, NULL, NULL); //akceptowanie oczekujących połączeń na gniazdach połączeniowych
                client* client = new_client(client_fd);
                if (client == NULL) {
                    printf("Server is full\n");
                    message msg = {.type = FULL_SERVER};
                    write(client_fd, &msg, sizeof(msg));
                    close(client_fd);
                    continue;
                }

                event_data* event_data_ptr = malloc(sizeof(event_data));
                event_data_ptr->type = CLIENT;
                event_data_ptr->payload.client = client;
                struct epoll_event event = {.events = EPOLLIN | EPOLLET , .data.ptr = event_data_ptr};

                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
            } 
            else if (data->type == CLIENT) {
                if (events[i].events & EPOLLHUP) {
                    pthread_mutex_lock(&mutex);
                    delete_client(data->payload.client);
                    pthread_mutex_unlock(&mutex);
                } 
                else {
                    on_client_message(data->payload.client);
                }
            }
        }
    }
}
