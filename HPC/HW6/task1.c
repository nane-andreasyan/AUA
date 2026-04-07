#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 100000000
#define NUM_THREADS 4

int global_sum_naive[256] = {0};
int global_sum_critical[256] = {0};
int global_sum_reduction[256] = {0};


int main() 
{
    int *arr = (int *)malloc(N * sizeof(int));
    if (arr == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    for (int i = 0; i < N; i++) {
        arr[i] = rand() % 256;
    }


    omp_set_num_threads(NUM_THREADS);

    //naive 
    #pragma omp parallel for 
    for (int i = 0; i < N; i++) {
        global_sum_naive[arr[i]]++;
    }

    //with critical 
    #pragma omp parallel 
    {
        int local_sum[256] = {0};

        #pragma omp for
        for (int i = 0; i < N; i++) {
            local_sum[arr[i]]++;
        }

        #pragma omp critical
        for (int j = 0; j < 256; j++) {
            global_sum_critical[j] += local_sum[j];
        }
    }

    //reduction 
    #pragma omp parallel for reduction(+:global_sum_reduction[:256])
    for (int i = 0; i < N; i++) {
        global_sum_reduction[arr[i]]++;
    }


    for (int i = 0; i < 256; i++) {
        printf("Value %d: Naive = %d, Critical = %d, Reduction = %d\n", i, 
            global_sum_naive[i], 
            global_sum_critical[i], 
            global_sum_reduction[i]);
    }

    free(arr);
    return 0;
}