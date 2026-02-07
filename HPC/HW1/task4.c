#include <stdio.h>

int main() {

    int x = 5;
    int *ptr = &x;
    int **double_ptr = &ptr;

    printf("Value of x using pointer: %d\n",*ptr);
    printf("Value of x using double pointer: %d\n",**double_ptr);

    return 0;

}
