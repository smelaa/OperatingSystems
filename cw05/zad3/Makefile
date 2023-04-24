CC = gcc -Wall
REPORT="report.txt"

all: clean test

main: 
	$(CC) integral_progs.c -lm -o integral_progs
	$(CC) integral_part.c -lm -o integral_part

test:
	make main
	echo "One named pipe & few programs" > $(REPORT)
	./integral_progs 1e-9 20000 >> $(REPORT)
	./integral_progs 1e-9 23000 >> $(REPORT)
	./integral_progs 1e-9 26000 >> $(REPORT)
	./integral_progs 1e-9 30000 >> $(REPORT)

clean:
	rm -f integral_progs integral_part