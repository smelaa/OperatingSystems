CC = gcc -Wall
REPORT="report.txt"

all: clean test

main: integral.c
	$(CC) integral.c -lm -o integral

test:
	make main
	echo "N pipes & few processes" > $(REPORT)
	./integral 1e-9 500 >> $(REPORT)
	./integral 1e-9 1000 >> $(REPORT)
	./integral 1e-8 1000 >> $(REPORT)
	./integral 1e-8 500 >> $(REPORT)

clean:
	rm -f integral 