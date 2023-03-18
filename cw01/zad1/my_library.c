#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "my_library.h"


/*
Utworzenie i zwrócenie struktury zawierającej: 
Zainicializowaną zerami tablicę wskaźników w której będą przechowywane wskaźniki na bloki pamięci.
Rozmiar tablicy, tj. maksymalna ilość bloków jakie można zapisać.
Aktualny rozmiar, tj. ilość zapisanych bloków.
*/
pointersArray createPointersArray(int size){
    pointersArray pArray;
    pArray.maxSize=size;
    pArray.currSize=0;
    pArray.array=calloc(size,sizeof(char*));
    return pArray;
}

/*
Przeprowadzenie procedury zliczania ilości linii i słów dla podanego pliku:
Procedura przyjmuje strukturę z pkt.1 oraz nazwę pliku.
Uruchomienie (system()) programu wc do zliczenia lini, słów i znaków dla zadanego pliku i przesłanie wyniku do pliku tymczasowego w katalogu /tmp. 
Zarezerwowanie bloku pamięci (calloc()) o rozmiarze odpowiadającym rzeczywistemu rozmiarowi danych znajdujących się w buforze tymczasowym i przeniesienie tych danych do nowo zaalokowanego bloku pamięci.
Inkrementację licznika ilości zapisanych bloków.
Usunięcie pliku tymczasowego.
*/
int countLinesAndWords(pointersArray* pArray, char fileName[]){
    //sprawdzanie czy jest miejsce w pArray
    if (pArray->currSize>=pArray->maxSize){
        fprintf(stderr, "No space left in pointersArray\n");
        return 1;
    }
    //tworzenie pliku tymczasowego
    char tempfile[20]="/tmp/tempXXXXXX";
    int temp=mkstemp(tempfile);
    if (temp==-1){
        fprintf(stderr, "Temporary file could not be created.\n");
        return 1;
    }
    //realizacja komendy wc
    char command[100]="wc ";
    strcat(command, fileName);
    strcat(command, " > ");
    strcat(command, tempfile);
    int sys_return = system(command);
    //sprawdzenie czy komenda sie wykonala poprawnie
    if (sys_return!=0){
        fprintf(stderr, "Error while executing %s.\n", command);
        remove(tempfile);
        return 1;
    }
    //otwieranie pliku tymczasowego
    FILE* tmp=fopen(tempfile,"r");
    if (!tmp){
        fprintf(stderr, "Error while opening temp file.\n");
        return 1;
    }
    //przesuniecie kursora na koniec
    fseek(tmp, 0, SEEK_END);
    //odczyt pozycji kursora
    long size=ftell(tmp);
    //przesuniecie kursora na poczatek
    fseek(tmp,0,SEEK_SET);
    //zaalokowanie pamieci dla danych z pliku
    pArray->array[pArray->currSize]=calloc(size,sizeof(char));
    //wczytanie danych do tablicy i zamkniecie pliku
    fread(pArray->array[pArray->currSize],sizeof(char),size,tmp);
    fclose(tmp);
    //usuniecie pliku tymczasowego
    remove(tempfile);
    //inkrementacja licznika zapisanych blokow
    pArray->currSize++;
    return 0;
}

/*
Zwrócenie zawartości bloku o zadanym indeksie. Procedura przyjmuje strukturę z pkt.1
*/
const char* returnBlock (pointersArray* pArray, int index){
    if(index>=pArray->currSize && index>=0){
        fprintf(stderr, "There is no data at index %i.\n", index);
        return NULL;
    }
    if(pArray->array[index]==NULL){
        fprintf(stderr, "Data at index %i has been deleted.\n", index);
        return NULL;
    }
    return pArray->array[index];
}

/*
Usunięcie z pamięci bloku o zadanym indeksie. Procedura przyjmuje strukturę z pkt.1
*/
int removeBlock (pointersArray* pArray, int index){
    free(pArray->array[index]);
    pArray->array[index]=NULL;
    return 0;
}

/*
Zwolnienie pamięci tablicy wskaźników.
*/
int removeArray(pointersArray* pArray){
    for(int i=0;i<pArray->maxSize;i++){
        free(pArray->array[i]);
        //if !=NULL??
    }
    free(pArray->array);
    pArray->currSize=0;
    pArray->maxSize=0;
    return 0;
}
