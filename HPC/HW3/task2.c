#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define THREADS 10

pthread_barrier_t barrier;

void * wait_at_barrier(void *arg) {
    printf("Getting ready...\n");
    sleep(3);
    pthread_barrier_wait(&barrier);
    return NULL;
}


int main() {

    pthread_barrier_init(&barrier, NULL, THREADS+1);
    pthread_t threads[THREADS];

    for (int i = 0; i < THREADS; i++) {
        if (pthread_create(&threads[i], NULL, wait_at_barrier, NULL) != 0) {
            perror("Thread creation failed\n");
            for (int j = 0; j < i; j++) {
                pthread_join(threads[j], NULL);
            }
            return 1;
        }
    }

    pthread_barrier_wait(&barrier);
    printf("Game Started!\n");


    for (int i = 0; i < THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Thread join failed\n");
            return 1;
        }
    }

    pthread_barrier_destroy(&barrier);
    return 0;


}



