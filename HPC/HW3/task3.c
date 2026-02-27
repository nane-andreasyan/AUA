#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define S 10

int data[S];
pthread_barrier_t barrier;


void * collect_data(void *arg) {
    int id = (int)(long)arg;
    data[id] = rand() % 10;
    printf("Sensor %d collected data: %d\n", id, data[id]); 
    pthread_barrier_wait(&barrier);
    return NULL;
}

int main() {
    pthread_t threads[S];
    pthread_barrier_init(&barrier, NULL, S+1);

    for (int i = 0; i < S; i++) {
        if(pthread_create(&threads[i], NULL, collect_data, (void *)(long)i)!=0) {
            perror("Thread creation failed\n");
            for (int j = 0; j < i; j++) {
                pthread_join(threads[j], NULL);
            }
            return 1;
        }
    }

    pthread_barrier_wait(&barrier);

    int sum = 0;

    for (int i = 0; i < S; i++) {
        sum += data[i];
    }

    double average = (double)sum / S;
    
    printf("Average: %.2f\n", average);

    for (int i = 0; i < S; i++) {
       if(pthread_join(threads[i], NULL)!=0) {
           perror("Thread join failed\n");
           return 1;
       }
    }

    pthread_barrier_destroy(&barrier);
    return 0;
}


