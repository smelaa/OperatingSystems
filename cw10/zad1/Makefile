CC=gcc
CFLAGS=-Wall

all:
	make client
	make server

client: client.c
	$(CC) $(CFLAGS) client.c -o client -lpthread

server: server.c
	$(CC) $(CFLAGS) server.c -o server -lpthread

clean:
	rm -f client server