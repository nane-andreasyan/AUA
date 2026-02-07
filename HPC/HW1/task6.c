#include <stdio.h>
#define STRING_VAL "abcdef"

int str_length(char *str) {
    int length = 0;
    while(*str != '\0') {
        length++;
        str++;
    }
    return length;
}

int main() {

    char* str = STRING_VAL;
    while(*str != '\0') {
        printf("%c", *str);
        str++;
    }
    printf("\n");
    printf("Length of the string: %d\n", str_length(STRING_VAL));

    return 0;
}
