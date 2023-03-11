STATIC_PROG=../zad2/main_static
SHARED_PROG=../zad2/main_shared
DYNAMIC_PROG=../zad2/main_dynamic

INPUT=input.txt

STATIC_RAPORT=results_static.txt
SHARED_RAPORT=results_shared.txt
DYNAMIC_RAPORT=results_dynamic.txt

RAPORT=raport.txt

regular: compile_main test_all

test_all: clean test_static test_shared test_dynamic cat_files

compile_main:
	make all -C ../zad2

test_static:
	echo "static: " > $(STATIC_RAPORT)
	cat $(INPUT) | $(STATIC_PROG) >> $(STATIC_RAPORT)

test_shared:
	echo "shared: " > $(SHARED_RAPORT)
	cat $(INPUT) | $(SHARED_PROG) >> $(SHARED_RAPORT)

test_dynamic:
	echo "dynamic: " > $(DYNAMIC_RAPORT)
	cat $(INPUT) | $(DYNAMIC_PROG) >> $(DYNAMIC_RAPORT)

cat_files:
	echo "ZAD3A: " > $(RAPORT)
	cat $(STATIC_RAPORT) $(SHARED_RAPORT) $(DYNAMIC_RAPORT) > $(RAPORT)

clean:
	rm -f $(STATIC_RAPORT) $(SHARED_RAPORT) $(DYNAMIC_RAPORT) $(RAPORT)
