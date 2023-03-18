#ifndef MY_LIBRARY_H
#define MY_LIBRARY_H

typedef struct pointersArray {
    char** array;
    int maxSize;
    int currSize;
}pointersArray;

pointersArray createPointersArray(int size);
int countLinesAndWords(pointersArray* pArray, char fileName[]);
const char* returnBlock (pointersArray* pArray, int index);
int removeBlock (pointersArray* pArray, int index);
int removeArray(pointersArray* pArray);

#endif