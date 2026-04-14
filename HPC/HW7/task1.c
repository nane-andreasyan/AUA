#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stdio.h>

#define NUM_THREADS 4

int fib(int n) {

    if( n < 2) {
        return n;
    }

    if(n < 10) {
        return fib(n - 1) + fib(n - 2);
    }

    int a, b, c;

    #pragma omp task shared(a)
    a = fib(n - 1);

    #pragma omp task shared(b)
    b = fib(n - 2);

    #pragma omp taskwait
    c = a + b;

    return c;
}

int main() {

    int num;
    printf("Enter the number to calculate Fibonacci: ");
    scanf("%d", &num);

     #pragma omp parallel num_threads(NUM_THREADS)
     {
        #pragma omp single
        {
            int result = fib(num);
            printf("Fib(%d) = %d\n", num, result);
        }
     }

    return 0;

}