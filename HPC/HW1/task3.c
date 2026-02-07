#include <stdio.h>

void swap(int *a, int *b) {
    // need to dereference the pointers
    int temp = *a;
    *a = *b;
    *b = temp;
}

int main() {
    int x = 5, y = 10;
    printf("Initial values of x: %d, y: %d\n", x, y);
    swap(&x, &y);
    printf("Swapped values of x: %d, y: %d\n", x, y);
    return 0;
}

