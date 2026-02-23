#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#define SIZE 50000000
#define THREADS 5

typedef struct
{
    int start;
    int end;
    int *arr;
} SharedParam;

void *sum_array(void *arg)
{
    SharedParam* args = (SharedParam *)arg;
    int start = args->start;
    int end = args->end;
    int *arr = args->arr;
    long long *sum = calloc(1, sizeof(long long));
    if (sum == NULL)
    {
        perror("Failed to allocate memory for sum");
        return NULL;
    }

    for (int i = start; i < end; i++)
    {
        *sum += arr[i];
    }
    return sum;
}

int main()
{

    pthread_t tids[THREADS];
    int *arr = malloc(SIZE * sizeof(int));
    if (arr == NULL)
    {
        perror("Failed to allocate memory");
        return 1;
    }

    for (int i = 0; i < SIZE; i++)
    {
        arr[i] = i + 1;
    }

    int offset = SIZE / THREADS;

    struct timespec start, end;
    double time_spent;

    clock_gettime(CLOCK_MONOTONIC, &start);

    SharedParam params[THREADS];
    for (int i = 0; i < THREADS; i++)
    {
        params[i].start = i * offset;
        if (i == THREADS - 1)
        {
            params[i].end = SIZE;
        }
        else
        {
            params[i].end = (i + 1) * offset;
        }
        params[i].arr = arr;
        if (pthread_create(&tids[i], NULL, sum_array, &params[i]) != 0)
        {
            perror("Failed to create thread");
            for (int j = 0; j < i; j++)
            {
                long long *res = NULL;
                pthread_join(tids[j], (void **)&res);
                free(res);
            }
            free(arr);
            return 1;
        }
    }

    long long total_sum = 0;

    for (int i = 0; i < THREADS; i++)
    {
        long long *result;
        if (pthread_join(tids[i], (void **)&result) != 0)
        {
            perror("Failed to join thread");
            for (int j = i + 1; j < THREADS; j++)
            {
                long long *res = NULL;
                pthread_join(tids[j], (void **)&res);
                free(res);
            }
            free(arr);
            return 1;
        }

        if (result == NULL)
        {
            fprintf(stderr, "Thread %d returned NULL \n", i);
            for (int j = i + 1; j < THREADS; j++)
            {
                long long *res = NULL;
                pthread_join(tids[j], (void **)&res);
                free(res);
            }
            free(arr);
            return 1;
        }
        total_sum += *result;
        free(result);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Total sum: %lld, time spent with threads %f seconds\n", total_sum, time_spent);

    total_sum = 0;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < SIZE; i++)
    {
        total_sum += arr[i];
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Total sum: %lld, time spent without threads %f seconds\n", total_sum, time_spent);

    free(arr);
    return 0;
}
