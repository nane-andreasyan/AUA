#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>


#define NUM_THREADS 4
#define N 10

long long balance = 0;
int use_mutex = 1;
int long_condition = 0;

pthread_mutex_t mutex;
pthread_spinlock_t spinlock;

void lock() {
    if (use_mutex)
        pthread_mutex_lock(&mutex);
    else
        pthread_spin_lock(&spinlock);
}

void unlock() {
    if (use_mutex)
        pthread_mutex_unlock(&mutex);
    else
        pthread_spin_unlock(&spinlock);
}

void* increment(void* arg) {
    for (int i = 0; i < N; i++) {
        lock();
        balance++;
        if(long_condition) {
           usleep(100);
        }
        unlock();
    }
    return NULL;
}

void* decrement(void* arg) {
    for (int i = 0; i < N; i++) {
        lock();
        balance--;
        if(long_condition) {
           usleep(100);
        }
        unlock();
    }
    return NULL;
}

int main(int argc, char** argv) {
    pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
    pthread_mutex_init(&mutex, NULL);
    pthread_t threads[NUM_THREADS*2];

    if (strcmp(argv[1], "mutex") == 0)
        use_mutex = 1;
    else if (strcmp(argv[1], "spin") == 0)
        use_mutex = 0;
    else {
        printf("Unknown lock type\n");
        return 1;
    }

    if (strcmp(argv[2], "long") == 0)
        long_condition = 1;
    else if (strcmp(argv[2], "short") == 0)
        long_condition = 0;
    else {
        printf("Unknown critical section type\n");
        return 1;
    }

    struct timeval  tv1, tv2;
    gettimeofday(&tv1, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, increment, NULL) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }


    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i+NUM_THREADS], NULL, decrement, NULL) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS*2; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Failed to join thread");
            return 1;
        }
    }

    gettimeofday(&tv2, NULL);

    if(use_mutex){
        pthread_mutex_destroy(&mutex);
    } else {
        pthread_spin_destroy(&spinlock);
    }
    printf("Final balance value: %lld\n", balance);
    printf ("Total time = %f seconds\n",
        (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
        (double) (tv2.tv_sec - tv1.tv_sec));

    return 0;
}
