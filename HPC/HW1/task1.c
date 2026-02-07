#include <stdio.h>

int main() {

    int x = 5;
    int* ptr = &x;

    printf("Address of x using variable: %p\n",&x);
    printf("Address of x using pointer: %p\n",ptr);

    printf("Value of x before modification: %d\n",x);

    *(ptr) = 10;

    printf("Value of x after modification: %d\n",x);

    return 0;
}
