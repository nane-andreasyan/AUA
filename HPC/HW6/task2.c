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

    double min_diff = arr[1] - arr[0];
    if (min_diff < 0) {
        min_diff = -min_diff;
    }

    #pragma omp parallel for reduction(min:min_diff)
    for (long long i = 1; i < N; i++) {
        double diff = arr[i] - arr[i-1];
        if (diff < 0) {
            diff = -diff;
        }
       if(diff < min_diff) {
           min_diff = diff;
       }
    }

    printf("Minimum difference:  %.12f\n", min_diff);

    free(arr);
    return 0;


}