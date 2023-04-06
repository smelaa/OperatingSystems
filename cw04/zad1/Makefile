CC = gcc
CFLAGS = -Wall
PROG_FORK="sigfork"
PROG_EXEC="sigexec"
REPORT="report2.txt"


all: exec fork 

exec: sigexec.c
	$(CC) $(CFLAGS) sigexec.c -o sigexec

fork: sigfork.c
	$(CC) $(CFLAGS) sigfork.c -o sigfork

test:
	make clean
	make all
	echo "### FORK" > $(REPORT)
	echo "IGNORE:" >> $(REPORT)
	./$(PROG_FORK) ignore >> $(REPORT)
	echo "HANDLER:" >> $(REPORT)
	./$(PROG_FORK) handler >> $(REPORT)
	echo "MASK:" >> $(REPORT)
	./$(PROG_FORK) mask >> $(REPORT)
	echo "PENDING:" >> $(REPORT)
	./$(PROG_FORK) pending >> $(REPORT)

	echo "\n### EXEC" >> $(REPORT)
	echo "IGNORE:" >> $(REPORT)
	./$(PROG_EXEC) ignore >> $(REPORT)
	echo "MASK:" >> $(REPORT)
	./$(PROG_EXEC) mask >> $(REPORT)
	echo "PENDING:" >> $(REPORT)
	./$(PROG_EXEC) pending >> $(REPORT)

#Wnioski:
#Po wykonaniu fork do procesu potomnego przekazywane sa wartosci maski i sposob obslugi sygnalow.
#W procesie potomnym nie ma natomiast oczekujacych sygnalow z procesu macierzystego.
#Po wykonaniu funkcji exec maski, sposob obslugi i oczekujace sygnaly sa takie same jak w procesie, ktory wywolal exec.

clean:
	rm -f sigfork sigexec