#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 50000000
#define NUM_THREADS 4

int main() {

    double *arr = (double *)malloc(N * sizeof(double));
    if (arr == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    for (long long i = 0; i < N; i++) {
        arr[i] = (double)rand() / RAND_MAX;
    }

    omp_set_num_threads(NUM_THREADS);

    double max = 0.0;
    #pragma omp parallel for reduction(max:max)
    for (long long i = 0; i < N; i++) {
        if(arr[i] > max) {
            max = arr[i];
        }
    }

    printf("Maximum value:  %.12f\n", max);

    double T = 0.8 * max;
    double sum = 0.0;

    printf("Value of T:  %.12f\n", T);

    #pragma omp parallel for reduction(+:sum)
    for (long long  i = 0; i < N; i++) {
        if (arr[i] > T) {
            sum += arr[i];
        }
    }
    printf("Sum of elements greater than T:  %.12f\n", sum);
    free(arr);
    return 0;


}