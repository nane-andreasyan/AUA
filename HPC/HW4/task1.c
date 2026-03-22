#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <pthread.h>
#include <immintrin.h>


#define NUM_THREADS 4

long A_count = 0;
long C_count = 0;
long G_count = 0;
long T_count = 0;

typedef struct {
    char *buff;
    long start;
    long end;
    int counts[4];
} ThreadData;

pthread_mutex_t lock;


char* read_file(const char* filename, long *size) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Failed to open file");
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    *size = ftell(fp);
    rewind(fp);

    char *buffer = malloc(*size);
    if (!buffer) {
        perror("Memory allocation failed");
        return NULL;
    }
    size_t read_bytes = fread(buffer, 1, *size, fp);
    if (read_bytes != *size) {
        perror("fread failed or incomplete read");
        free(buffer);
        return NULL;
    }
    fclose(fp);
    return buffer;
}


void scalar_count(char *buff, long size, int *counts) {
    for (long i = 0; i < size; i++) {
        if (buff[i] == 'A') {
            counts[0]++;
        } else if (buff[i] == 'C') {
            counts[1]++;
        } else if (buff[i] == 'G') {
            counts[2]++;
        } else if (buff[i] == 'T') {
            counts[3]++;
        }
    }
}

void* threaded_count(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    char *buff = data->buff;
    long start = data->start;
    long end = data->end;

    for (long i = start; i < end; i++) {
        if (buff[i]  == 'A') {
            pthread_mutex_lock(&lock);
            A_count++;
            pthread_mutex_unlock(&lock);
        } else if (buff[i] == 'C') {
            pthread_mutex_lock(&lock);
            C_count++;
            pthread_mutex_unlock(&lock);
        } else if (buff[i] == 'G') {
            pthread_mutex_lock(&lock);
            G_count++;
            pthread_mutex_unlock(&lock);
        } else {
            pthread_mutex_lock(&lock);
            T_count++;
            pthread_mutex_unlock(&lock);
        }
    }
    return NULL;
}


void simd_count(char *buff, long size, int counts[4]) {
    __m256i A_vec = _mm256_set1_epi8('A');
    __m256i C_vec = _mm256_set1_epi8('C');
    __m256i G_vec = _mm256_set1_epi8('G');
    __m256i T_vec = _mm256_set1_epi8('T');

    long i;

    for (i = 0; i <= size-32; i += 32) {
        __m256i data_vec = _mm256_loadu_si256((__m256i*)(buff + i));

        __m256i A_mask = _mm256_cmpeq_epi8(data_vec, A_vec);
        __m256i C_mask = _mm256_cmpeq_epi8(data_vec, C_vec);
        __m256i G_mask = _mm256_cmpeq_epi8(data_vec, G_vec);
        __m256i T_mask = _mm256_cmpeq_epi8(data_vec, T_vec);

        counts[0] += __builtin_popcount(_mm256_movemask_epi8(A_mask));
        counts[1] += __builtin_popcount(_mm256_movemask_epi8(C_mask));
        counts[2] += __builtin_popcount(_mm256_movemask_epi8(G_mask));
        counts[3] += __builtin_popcount(_mm256_movemask_epi8(T_mask));
    }

    for(; i < size; i++) {
        if (buff[i] == 'A') {
            counts[0]++;
        } else if (buff[i] == 'C') {
            counts[1]++;
        } else if (buff[i] == 'G') {
            counts[2]++;
        } else if (buff[i] == 'T') {
            counts[3]++;
        }
    }
}


void* threaded_count_simd(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    char *buff = data->buff;
    long start = data->start;
    long end = data->end;
    int* local_counts = data->counts;
    simd_count(buff + start, end - start, local_counts);
    return NULL;
}


int main() {

    long size;
    char *buffer = read_file("dna.txt", &size);
    if (!buffer) {
        perror("Failed to read file");
        return 1;
    }

    int counts[4] = {0, 0, 0, 0};

    printf("DNA Size: %ld MB\n", size/1048576);
    printf("Threads used: %d\n", NUM_THREADS);


    struct timespec start, end; 
    double elapsed_time;


    // scalar

    clock_gettime(CLOCK_MONOTONIC, &start);
    scalar_count(buffer, size, counts);
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Counts (A C G T):\n");
    printf("%d, %d, %d, %d\n", counts[0], counts[1], counts[2], counts[3]);
    printf("Scalar Time: %lf sec\n", elapsed_time);

    long chunk_size = size / NUM_THREADS;
    chunk_size = (chunk_size / 64) * 64;

    // Multithreading

    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].buff = buffer;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i + 1) * chunk_size;
        if (i == NUM_THREADS - 1) {
            thread_data[i].end = size;
        }
        if (pthread_create(&threads[i], NULL, threaded_count, &thread_data[i]) != 0) {
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
    simd_count(buffer, size, counts);
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("SIMD Time: %lf sec\n", elapsed_time);


    // SIMD + Multithreading

    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_t threads2[NUM_THREADS];
    ThreadData thread_data2[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data2[i].buff = buffer;
        thread_data2[i].start = i * chunk_size;
        thread_data2[i].end = (i + 1) * chunk_size;
        for (int j = 0; j < 4; j++) {
            thread_data2[i].counts[j] = 0;
        }
        if (i == NUM_THREADS - 1) {
            thread_data2[i].end = size;
        }
        if (pthread_create(&threads2[i], NULL, threaded_count_simd, &thread_data2[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads2[i], NULL) != 0){
            perror("Failed to join thread");
            return 1;
        };
        A_count += thread_data2[i].counts[0];
        C_count += thread_data2[i].counts[1];
        G_count += thread_data2[i].counts[2];
        T_count += thread_data2[i].counts[3];
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("SIMD + Multithreading time: %lf sec\n", elapsed_time);

    free(buffer);
    return 0;
}