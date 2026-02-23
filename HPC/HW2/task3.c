#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>

#define SIZE 50000000
#define THREADS 4

typedef struct
{
    long long max;
    int start;
    int end;
    int *arr;
} SharedParam;

void *find_max(void *arg)
{
    SharedParam *args = (SharedParam *)arg;
    int start = args->start;
    int end = args->end;
    int *arr = args->arr;
    args->max = arr[start];

    for (int i = start; i < end; i++)
    {
        if (arr[i] > args->max)
        {
            args->max = arr[i];
        }
    }
    return NULL;
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
        if (pthread_create(&tids[i], NULL, find_max, &params[i]) != 0)
        {
            perror("Failed to create thread");
            for (int j = 0; j < i; j++)
            {
                pthread_join(tids[j], NULL);
            }
            free(arr);
            return 1;
        }
    }

    long long max = LLONG_MIN;

    for (int i = 0; i < THREADS; i++)
    {
        if (pthread_join(tids[i], NULL) != 0)
        {
            perror("Failed to join thread");
            for (int j = i + 1; j < THREADS; j++)
            {
                pthread_join(tids[j], NULL);
            }
            free(arr);
            return 1;
        }

        if (params[i].max > max)
        {
            max = params[i].max;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Max number is : %lld, time spent with threads %f seconds\n", max, time_spent);

    max = arr[0];
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < SIZE; i++)
    {
        if (arr[i] > max)
        {
            max = arr[i];
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Max number is : %lld, time spent without threads %f seconds\n", max, time_spent);

    free(arr);
    return 0;
}
