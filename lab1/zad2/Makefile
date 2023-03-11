CC = gcc
CFLAGS = -Wall

regular: make_library static shared dynamic static_variants shared_variants dynamic_variants

all: make_library static shared dynamic

all_variants: make_library static_variants shared_variants dynamic_variants

make_library:
	make -C ../zad1

static: main.c
	$(CC) $(CFLAGS) main.c ../zad1/my_library.a -o main_static

shared: main.c
	$(CC) $(CFLAGS) main.c -lmy_library -L../zad1/ -Wl,-rpath,../zad1/ -o main_shared

dynamic: main.c
	$(CC) $(CFLAGS) main.c -o main_dynamic -ldl -D DLL -I.

static_variants: main.c
	$(CC) $(CFLAGS) main.c ../zad1/my_library.a -o main_static_O0 -O0
	$(CC) $(CFLAGS) main.c ../zad1/my_library.a -o main_static_O -O
	$(CC) $(CFLAGS) main.c ../zad1/my_library.a -o main_static_O1 -O1
	$(CC) $(CFLAGS) main.c ../zad1/my_library.a -o main_static_O2 -O2
	$(CC) $(CFLAGS) main.c ../zad1/my_library.a -o main_static_O3 -O3
	$(CC) $(CFLAGS) main.c ../zad1/my_library.a -o main_static_Os -Os

shared_variants: main.c
	$(CC) $(CFLAGS) main.c -lmy_library -L../zad1/ -Wl,-rpath,../zad1/ -o main_shared_O0 -O0
	$(CC) $(CFLAGS) main.c -lmy_library -L../zad1/ -Wl,-rpath,../zad1/ -o main_shared_O -O
	$(CC) $(CFLAGS) main.c -lmy_library -L../zad1/ -Wl,-rpath,../zad1/ -o main_shared_O1 -O1
	$(CC) $(CFLAGS) main.c -lmy_library -L../zad1/ -Wl,-rpath,../zad1/ -o main_shared_O2 -O2
	$(CC) $(CFLAGS) main.c -lmy_library -L../zad1/ -Wl,-rpath,../zad1/ -o main_shared_O3 -O3
	$(CC) $(CFLAGS) main.c -lmy_library -L../zad1/ -Wl,-rpath,../zad1/ -o main_shared_Os -Os

dynamic_variants: main.c
	$(CC) $(CFLAGS) main.c -o main_dynamic_O0 -ldl -D DLL -I. -O0
	$(CC) $(CFLAGS) main.c -o main_dynamic_O -ldl -D DLL -I. -O
	$(CC) $(CFLAGS) main.c -o main_dynamic_O1 -ldl -D DLL -I. -O1
	$(CC) $(CFLAGS) main.c -o main_dynamic_O2 -ldl -D DLL -I. -O2
	$(CC) $(CFLAGS) main.c -o main_dynamic_O3 -ldl -D DLL -I. -O3
	$(CC) $(CFLAGS) main.c -o main_dynamic_Os -ldl -D DLL -I. -Os

clean:
	rm -f main_*