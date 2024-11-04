/* Header info for vector.c. The Vector struct contains the data necessary; but
   since C is not object-oriented, the traditional "methods" that you're
   accustomed to are instead functions that take a pointer to that struct as a
   parameter. */

struct charVector
{
    // A pointer to a traditional C array to hold data
    char* array;

    // The size of the actual C array that you have allocated
    int memorySize;

    // The virtual size of the vector, i.e. how much data is in it
    // from the user's perspective
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
void init(charVector *vector, int memorySize);

/* Removes the array associated with the Vector. */
void cleanup(charVector *vector);

/* Print out the Vector for debugging purposes. */
void print(charVector *vector);

int append(charVector *vector, char *toAppend);



