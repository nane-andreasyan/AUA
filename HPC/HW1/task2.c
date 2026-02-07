#include <stdio.h>
#define SIZE 5

int main() {

    int arr[SIZE];
    int *ptr = arr;

    for( int i = 0; i < SIZE; i++) {
        arr[i] = i;
    }

    for(int i = 0; i < SIZE; i++) {
        printf("%d, ", *(ptr+i));
    }

    printf("\n");
    printf("Doubling the values in the array\n");
    for(int i = 0; i < SIZE; i++) {
        *(ptr+i) = *(ptr+i)*2;
    }
    printf("Printing using pointer ");

    for(int i = 0; i < SIZE; i++) {
        printf("%d, ", *(ptr+i));
    }
    printf("\n");

    printf("Printing using array name ");

    for(int i = 0; i < SIZE; i++) {
        printf("%d, ", arr[i]);
    }

    printf("\n");

    return 0;
}
