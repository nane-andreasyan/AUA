#include <stdio.h>
#include <pthread.h>


#define M 4
#define N 1000000

long long counter = 0;
pthread_spinlock_t spinlock;  

void* increment(void* arg) {
    for (int i = 0; i < N; i++) {
        pthread_spin_lock(&spinlock);
        counter++;
        pthread_spin_unlock(&spinlock);
    }
    return NULL;
}

int main() {

    pthread_t threads[M];
    pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
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

    pthread_spin_destroy(&spinlock);
    printf("M*N: %d\n", M*N);
    printf("Final counter value: %lld\n", counter);
    return 0;

}
