STATIC_PROGO0=../zad2/main_static_O0
STATIC_PROGO=../zad2/main_static_O
STATIC_PROGO1=../zad2/main_static_O1
STATIC_PROGO2=../zad2/main_static_O2
STATIC_PROGO3=../zad2/main_static_O3
STATIC_PROGOs=../zad2/main_static_Os

SHARED_PROGO0=../zad2/main_shared_O0
SHARED_PROGO=../zad2/main_shared_O
SHARED_PROGO1=../zad2/main_shared_O1
SHARED_PROGO2=../zad2/main_shared_O2
SHARED_PROGO3=../zad2/main_shared_O3
SHARED_PROGOs=../zad2/main_shared_Os

DYNAMIC_PROGO0=../zad2/main_dynamic_O0
DYNAMIC_PROGO=../zad2/main_dynamic_O
DYNAMIC_PROGO1=../zad2/main_dynamic_O1
DYNAMIC_PROGO2=../zad2/main_dynamic_O2
DYNAMIC_PROGO3=../zad2/main_dynamic_O3
DYNAMIC_PROGOs=../zad2/main_dynamic_Os

INPUT=input.txt

STATIC_RAPORTO0=results_static_O0.txt
STATIC_RAPORTO=results_static_O.txt
STATIC_RAPORTO1=results_static_O1.txt
STATIC_RAPORTO2=results_static_O2.txt
STATIC_RAPORTO3=results_static_O3.txt
STATIC_RAPORTOs=results_static_Os.txt

SHARED_RAPORTO0=results_shared_O0.txt
SHARED_RAPORTO=results_shared_O.txt
SHARED_RAPORTO1=results_shared_O1.txt
SHARED_RAPORTO2=results_shared_O2.txt
SHARED_RAPORTO3=results_shared_O3.txt
SHARED_RAPORTOs=results_shared_Os.txt

DYNAMIC_RAPORTO0=results_dynamic_O0.txt
DYNAMIC_RAPORTO=results_dynamic_O.txt
DYNAMIC_RAPORTO1=results_dynamic_O1.txt
DYNAMIC_RAPORTO2=results_dynamic_O2.txt
DYNAMIC_RAPORTO3=results_dynamic_O3.txt
DYNAMIC_RAPORTOs=results_dynamic_Os.txt

RAPORT=raport.txt

regular: compile_main test_all

test_all: clean test_static test_shared test_dynamic cat_files

compile_main:
	make all_variants -C ../zad2

test_static:
	echo "static_O0: " > $(STATIC_RAPORTO0) 
	cat $(INPUT) | $(STATIC_PROGO0) >> $(STATIC_RAPORTO0)

	echo "static_O: " > $(STATIC_RAPORTO) 
	cat $(INPUT) | $(STATIC_PROGO) >> $(STATIC_RAPORTO)

	echo "static_O1: " > $(STATIC_RAPORTO1) 
	cat $(INPUT) | $(STATIC_PROGO1) >> $(STATIC_RAPORTO1)

	echo "static_O2: " > $(STATIC_RAPORTO2) 
	cat $(INPUT) | $(STATIC_PROGO2) >> $(STATIC_RAPORTO2)

	echo "static_O3: " > $(STATIC_RAPORTO3) 
	cat $(INPUT) | $(STATIC_PROGO3) >> $(STATIC_RAPORTO3)

	echo "static_Os: " > $(STATIC_RAPORTOs) 
	cat $(INPUT) | $(STATIC_PROGOs) >> $(STATIC_RAPORTOs)

test_shared:
	echo "shared_O0: " > $(SHARED_RAPORTO0)
	cat $(INPUT) | $(SHARED_PROGO0) >> $(SHARED_RAPORTO0)

	echo "shared_O: " > $(SHARED_RAPORTO)
	cat $(INPUT) | $(SHARED_PROGO0) >> $(SHARED_RAPORTO)

	echo "shared_O1: " > $(SHARED_RAPORTO1)
	cat $(INPUT) | $(SHARED_PROGO1) >> $(SHARED_RAPORTO1)

	echo "shared_O2: " > $(SHARED_RAPORTO2)
	cat $(INPUT) | $(SHARED_PROGO2) >> $(SHARED_RAPORTO2)

	echo "shared_O3: " > $(SHARED_RAPORTO3)
	cat $(INPUT) | $(SHARED_PROGO3) >> $(SHARED_RAPORTO3)

	echo "shared_Os: " > $(SHARED_RAPORTOs)
	cat $(INPUT) | $(SHARED_PROGOs) >> $(SHARED_RAPORTOs)

test_dynamic:
	echo "dynamic_O0: " > $(DYNAMIC_RAPORTO0)
	cat $(INPUT) | $(DYNAMIC_PROGO0) >> $(DYNAMIC_RAPORTO0)

	echo "dynamic_O: " > $(DYNAMIC_RAPORTO)
	cat $(INPUT) | $(DYNAMIC_PROGO) >> $(DYNAMIC_RAPORTO)

	echo "dynamic_O1: " > $(DYNAMIC_RAPORTO1)
	cat $(INPUT) | $(DYNAMIC_PROGO1) >> $(DYNAMIC_RAPORTO1)

	echo "dynamic_O2: " > $(DYNAMIC_RAPORTO2)
	cat $(INPUT) | $(DYNAMIC_PROGO2) >> $(DYNAMIC_RAPORTO2)

	echo "dynamic_O3: " > $(DYNAMIC_RAPORTO3)
	cat $(INPUT) | $(DYNAMIC_PROGO3) >> $(DYNAMIC_RAPORTO3)

	echo "dynamic_Os: " > $(DYNAMIC_RAPORTOs)
	cat $(INPUT) | $(DYNAMIC_PROGOs) >> $(DYNAMIC_RAPORTOs)

cat_files:
	cat $(STATIC_RAPORTO0) $(STATIC_RAPORTO) $(STATIC_RAPORTO1) $(STATIC_RAPORTO2) $(STATIC_RAPORTO3) $(STATIC_RAPORTOs) > static.txt
	cat $(SHARED_RAPORTO0) $(SHARED_RAPORTO) $(SHARED_RAPORTO1) $(SHARED_RAPORTO2) $(SHARED_RAPORTO3) $(SHARED_RAPORTOs) > shared.txt
	cat $(DYNAMIC_RAPORTO0) $(DYNAMIC_RAPORTO) $(DYNAMIC_RAPORTO1) $(DYNAMIC_RAPORTO2) $(DYNAMIC_RAPORTO3) $(DYNAMIC_RAPORTOs) > dynamic.txt
	echo "ZAD3B: " > $(RAPORT)
	cat static.txt shared.txt dynamic.txt > $(RAPORT)
	rm -f static.txt shared.txt dynamic.txt

clean:
	rm -f $(STATIC_RAPORTO0) $(STATIC_RAPORTO) $(STATIC_RAPORTO1) $(STATIC_RAPORTO2) $(STATIC_RAPORTO3) $(STATIC_RAPORTOs)
	rm -f $(SHARED_RAPORTO0) $(SHARED_RAPORTO) $(SHARED_RAPORTO1) $(SHARED_RAPORTO2) $(SHARED_RAPORTO3) $(SHARED_RAPORTOs)
	rm -f $(DYNAMIC_RAPORTO0) $(DYNAMIC_RAPORTO) $(DYNAMIC_RAPORTO1) $(DYNAMIC_RAPORTO2) $(DYNAMIC_RAPORTO3) $(DYNAMIC_RAPORTOs)
	rm -f $(RAPORT)