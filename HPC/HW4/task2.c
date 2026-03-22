#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <pthread.h>
#include <immintrin.h>
#include <string.h>


#define NUM_THREADS 4
#define SIZE 209715200L

typedef struct {
    char *buff;
    long start;
    long end;
} ThreadData;


void* upper_thread(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    char *buff = data->buff;
    long start = data->start;
    long end = data->end;

    for (long i = start; i < end; i++) {
        if (buff[i] >= 'a' && buff[i] <= 'z') {
            buff[i] -= 32;
        }
    }
    return NULL;
}

void simd_upper(char *buff, long size) {

    __m256i start_char = _mm256_set1_epi8('a'-1);
    __m256i end_char = _mm256_set1_epi8('z');

    long i;

    for (i = 0; i <= size-32; i += 32) {
        __m256i data_vec = _mm256_loadu_si256((__m256i*)(buff + i));
        __m256i lowecase_start = _mm256_cmpgt_epi8(data_vec, start_char);
        __m256i lowecase_end = _mm256_cmpgt_epi8(data_vec, end_char);

        //satisfy both
        __m256i mask = _mm256_andnot_si256(lowecase_end, lowecase_start);

        __m256i int_mask = _mm256_and_si256(mask, _mm256_set1_epi8(32));

        //convert to uppercase
        __m256i upper_vec = _mm256_sub_epi8(data_vec, int_mask);
        
        _mm256_storeu_si256((__m256i *)(buff + i), upper_vec);
    }

    for(; i < size; i++) {
        if (buff[i] >= 'a' && buff[i] <= 'z') {
            buff[i] -= 32;
        }
    }
}

void * threaded_simd(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    char *buff = data->buff;
    long start = data->start;
    long end = data->end;

    simd_upper(buff + start, end - start);
    return NULL;
}


void print_buffer(const char *label, const char *buff, long size) {
    printf("%s:\n", label);

    for (long i = 0; i < size; i++) {
        putchar(buff[i]);
    }

    printf("\n\n");
}


int main() {


    char *buffer_thread = malloc(SIZE);
    char *buffer_simd = malloc(SIZE);
    char *buffer_simd_thread = malloc(SIZE);

    if (!buffer_thread || !buffer_simd || !buffer_simd_thread) {
        perror("Memory allocation failed\n");
        free(buffer_thread);
        free(buffer_simd);
        free(buffer_simd_thread);
        return 1;
    }

    const char charset[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789"
        "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~ ";

    for (int i = 0; i < SIZE; i++) {
        buffer_thread[i] = charset[rand() % (sizeof(charset) - 1)];
    }

    memcpy(buffer_simd, buffer_thread, SIZE);
    memcpy(buffer_simd_thread, buffer_thread, SIZE);


    printf("Buffer size: %ld MB\n", SIZE/1048576);
    printf("Threads used: %d\n\n", NUM_THREADS);

    print_buffer("Buffer BEFORE (sample)", buffer_thread, 50);

    long chunk_size = SIZE / NUM_THREADS;
    chunk_size = (chunk_size / 64) * 64;
    struct timespec start, end; 
    double elapsed_time;

    // Multithreading 
    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].buff = buffer_thread;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i + 1) * chunk_size;
        if (i == NUM_THREADS - 1) {
            thread_data[i].end = SIZE;
        }
        if (pthread_create(&threads[i], NULL, upper_thread, &thread_data[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0){
            perror("Failed to join thread");
            return 1;
        };
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Multithreading Time: %lf sec\n", elapsed_time);


    // SIMD 

    clock_gettime(CLOCK_MONOTONIC, &start);
    simd_upper(buffer_simd, SIZE);
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("SIMD Time: %lf sec\n", elapsed_time);


    // SIMD + Multithreading

    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_t threads_2[NUM_THREADS];
    ThreadData thread2_data[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread2_data[i].buff = buffer_simd_thread;
        thread2_data[i].start = i * chunk_size;
        thread2_data[i].end = (i + 1) * chunk_size;
        if (i == NUM_THREADS - 1) {
            thread2_data[i].end = SIZE;
        }
        if (pthread_create(&threads_2[i], NULL, threaded_simd, &thread2_data[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads_2[i], NULL) != 0){
            perror("Failed to join thread");
            return 1;
        };
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("SIMD + Multithreading Time: %lf sec\n\n", elapsed_time);

    print_buffer("Thread AFTER (sample)", buffer_thread, 50);
    print_buffer("SIMD AFTER (sample)", buffer_simd, 50);
    print_buffer("SIMD + Multithreading AFTER (sample)", buffer_simd_thread, 50);

    free(buffer_thread);
    free(buffer_simd);
    free(buffer_simd_thread);
    return 0;
}