CC = gcc
CFLAGS = -Wall

all: static shared

static: my_library.c
	$(CC) $(CFLAGS) -c my_library.c
	ar rcs my_library.a my_library.o
	rm -f my_library.o

shared: my_library.c
	$(CC) $(CFLAGS) -fPIC -c my_library.c 
	$(CC) $(CFLAGS) -shared -o libmy_library.so  my_library.o
	rm -f my_library.o

clean:
	rm -f *.a *.o *.so