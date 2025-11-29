#include <pthread.h>
#include <stdio.h>

#define M 4
#define N 1000000

long long counter = 0;
pthread_mutex_t mutex;

void* increment(void* arg) {
    for (int i = 0; i < N; i++) {
        pthread_mutex_lock(&mutex);
        counter++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {

    pthread_t threads[M];
    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < M; i++) {
        if (pthread_create(&threads[i], NULL, increment, NULL) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    for (int i = 0; i < M; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Failed to join thread");
            return 1;
        }
    }

    pthread_mutex_destroy(&mutex);
    printf("M*N: %d\n", M*N);
    printf("Final counter value: %lld\n", counter);
    return 0;

}
