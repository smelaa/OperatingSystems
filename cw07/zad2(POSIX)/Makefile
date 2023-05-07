CC = gcc -Wall

main: 
	$(CC) -c hairdresser.c 
	$(CC) -c client.c 
	$(CC) -c sem.c
	$(CC) -c queue.c
	$(CC) -c engine.c
	$(CC) engine.o sem.o queue.o -o engine
	$(CC) hairdresser.o sem.o queue.o -o hairdresser
	$(CC) client.o sem.o queue.o -o client
	rm -f client.o engine.o sem.o queue.o hairdresser.o

clean:
	rm -f engine hairdresser client
