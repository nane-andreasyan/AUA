#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t semA, semB, semC;
#define N 4

void* A(void* arg) {
    for (int i = 0; i < N; i++) {
        sem_wait(&semA);
        printf("A %d\n", i);
        sem_post(&semB);
    }
    return NULL;
}

void* B(void* arg) {
    for (int i = 0; i < N; i++) {
        sem_wait(&semB);
        printf("B %d\n", i);
        sem_post(&semC);
    }
    return NULL;
}

void* C(void* arg) {
    for (int i = 0; i < N; i++) {
        sem_wait(&semC);
        printf("C %d\n", i);
        sem_post(&semA);
    }
    return NULL;
}


int main() {
    pthread_t thread_A, thread_B, thread_C;

    sem_init(&semA, 0, 1); 
    sem_init(&semB, 0, 0);
    sem_init(&semC, 0, 0);


    if(pthread_create(&thread_A, NULL, A, NULL)!=0) {
        perror("Failed to create thread A");
        return 1;
    }

    if(pthread_create(&thread_B, NULL, B, NULL)!=0) {
        perror("Failed to create thread B");
        return 1;
    }

    if(pthread_create(&thread_C, NULL, C, NULL)!=0) {
        perror("Failed to create thread C");
        return 1;
    }

    if(pthread_join(thread_A, NULL)!=0) {
        perror("Failed to join thread A");
        return 1;
    }
    if(pthread_join(thread_B, NULL)!=0) {
        perror("Failed to join thread B");
        return 1;
    }
    if(pthread_join(thread_C, NULL)!=0) {
        perror("Failed to join thread C");
        return 1;
    }

    sem_destroy(&semA);
    sem_destroy(&semB);
    sem_destroy(&semC);

    return 0;
}
