/* Header info for vector.c. The Vector struct contains the data necessary; but
   since C is not object-oriented, the traditional "methods" that you're
   accustomed to are instead functions that take a pointer to that struct as a
   parameter. */

#include <stdlib.h>
#include <stdio.h>

#define SUCCESS 0
#define ARRAY_FULL 1

struct charVector
{
    // A pointer to a traditional C array to hold data
    int* array;

    // The size of the actual C array that you have allocated
    int memorySize;

    // The virtual size of the vector, i.e. how much data is in it
    // from the user's perspective. (Doesn't include null terminator)
    int size;
};

/* Typing "struct Vector" all the time is cumbersome, this sets it up so you can
 just type "Vector" instead. */

typedef struct charVector charVector;

/* Function prototypes. */

/* Take an already existing Vector as a parameter. (Do not create a Vector
 inside this function. Set its internal memory size to match the value given in
 the parameter, allocate an appropriately-sized C array to hold the actual data,
 and initialize the size variable to 0. */

void init(charVector *vector, int memorySize) {
    vector->size = 0;
    vector->memorySize = memorySize;
    vector->array = (char *) malloc(sizeof(char)*memorySize);
}


/* Removes the array associated with the Vector. */
void cleanup(charVector *vector) {
    // for (int i = 0; i < vector->memorySize; i++) {
    //     free(&(vector->array[i]));
    // }
    free(vector->array);
}


/* Print out the Vector for debugging purposes. */

void print(charVector *vector) {
    for (int i = 0; i < vector->size; i++) {
        printf("%s\n", vector->array);
    }
}

/*Appends *length* characters to the char array
dynamic: 1 will resize array if length exceeds memory size. 0 will append up to memorysize.
*/
int append(charVector *vector, char *toAppend, int length, int dynamic) {
    if (dynamic) {

    } else {
        if (length + vector->size -1 < vector->memorySize) {
            strncpy(vector->array + vector->size - 1, toAppend, length); //-1 to replace \0
            vector->size += length;
            return SUCCESS;
        } else { 
            strncpy(vector->array + vector->size, toAppend, vector->memorySize - vector->size);
            vector->array[vector->memorySize - 1] = '\0';
            vector->size = vector->memorySize;
            return ARRAY_FULL;
        }
    }
}

/* Insert value at location inside the Vector. If the list has items in
 positions 0 through n-1, you are only permitted to insert new values at
 locations 0 through n. (This is identical to Java's ArrayList behavior.) If
 there is already data at the location you are inserting into, insert slides the
 rest of the data down one position to make room for the new value. Returns 1 if
 success, and 0 if the location is invalid (less than 0, or greater than the
 size). Inserting at the very end of the current data (at position equal to
 size) is fine. All inserts should increase size by 1.

 If the internal array is too small to fit a new value, a new array of twice the
 size is created with malloc. The values are copied, the Vector is appropriately
 updated, and the old array is freed.) */

int insert(charVector *vector, int location, int value) {
    
    if (location >= vector->size + 1 || location < 0) {
        // printf("Invalid location: location must be 0 =< x =< size\n");
        return 0;
    } 
    
    //If we need to resize
    if (vector->size >= vector->memorySize) {
        int* newArray = realloc(vector->array, sizeof(int) *(vector->memorySize) * 2);
        
        if (newArray == NULL) { //If realloc fails, free newArray.
            free(newArray);
            return 0;
        } 
        else {
            vector->array = newArray;
        }
        vector->memorySize = vector->memorySize * 2;
    }
    
    for (int i = vector->size; i > location; i--) {
        vector->array[i] = vector->array[i-1];
    }
    vector->array[location] = value;
    vector->size++;
    return 1;
}


