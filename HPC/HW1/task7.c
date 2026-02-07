#include <stdio.h>

int main() {

    char* arr[] = {"hi", "how", "are", "you"};

    // printing using pointer arithmetic
    // for (int i = 0; i < 4; i++) {
    //     char *p = *(arr + i);

    //     while (*p != '\0') {
    //         printf("%c", *p);
    //         p++;
    //     }
    //     printf(" ");
    // }
    // printf("\n");

    for(int i = 0; i < 4; i++) {
        printf("%s ", *(arr+i));
    }

    printf("\n\n");

    arr[0]  = "hi there";
    
    printf("After modifying the first element of the array\n\n");

    for(int i = 0; i < 4; i++) {
        printf("%s ", *(arr+i));
    }
    printf("\n");

    return 0;
}
