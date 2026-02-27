#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


pthread_barrier_t barrier;
#define M 5

void * thread_pipeline(void *arg) {
    int id = (int)(long)arg;

    printf("Thread %d: Stage 1\n", id);
    sleep(1); // Simulate work
    pthread_barrier_wait(&barrier);

    printf("Thread %d: Stage 2\n", id);
    sleep(1); // Simulate work
    pthread_barrier_wait(&barrier);

    printf("Thread %d: Stage 3\n", id);
    sleep(1); // Simulate work
    pthread_barrier_wait(&barrier);

    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t threads[M];
    pthread_barrier_init(&barrier, NULL, M);

    for (int i = 0; i < M; i++) {
        if (pthread_create(&threads[i], NULL, thread_pipeline, (void *)(long)i) != 0) {
            perror("Thread creation failed\n");
            for (int j = 0; j < i; j++) {
                pthread_join(threads[j], NULL);
            }
            return 1;
        }
    }

    for (int i = 0; i < M; i++) {
       if(pthread_join(threads[i], NULL)!=0) {
           perror("Thread join failed\n");
           return 1;
       }
    }

    pthread_barrier_destroy(&barrier);
    return 0;

}
