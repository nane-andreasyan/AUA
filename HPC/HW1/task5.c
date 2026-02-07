#include <stdio.h>
#include <stdlib.h>
#define SIZE 5

int main() {

    int* ptr = malloc(sizeof(int));
    
    if (ptr == NULL) {
        printf("failed to allocate memory\n");
        return 1;
    }

    *ptr = 5;
    printf("Value of x: %d\n", *ptr);
    free(ptr);
    ptr = NULL;


    printf("\n");
    int* arr = malloc(SIZE * sizeof(int));

    if (arr == NULL) {
        printf("failed to allocate memory\n");
        return 1;
    }

    for(int i = 0; i < SIZE; i++) {
        *(arr+i) = i;
        printf("Value at %d: %d\n", i, *(arr+i));
    }

    free(arr);
    arr = NULL;

    return 0;

}
