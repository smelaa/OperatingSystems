CC = gcc
CFLAGS = -Wall
REPORT="report.txt"

main: sigaction.c
	$(CC) $(CFLAGS) sigaction.c -o sigaction

test:
	echo report.txt to przekopiowany output, gdyz Makefile nie byl w stanie wygenerowac raportu ze wzgledu na test flagi SA_RESETHAND

clean:
	rm -f sigaction