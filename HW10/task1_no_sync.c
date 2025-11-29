#include <pthread.h>
#include <stdio.h>

#define M 4
#define N 1000000

long long counter = 0;

void* increment(void* arg) {
    for (int i = 0; i < N; i++) {
        counter++;
    }
    return NULL;
}


int main() {

    pthread_t threads[M];
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

    printf("M*N: %d\n", M*N);
    printf("Final counter value: %lld\n", counter);
    return 0;

}
