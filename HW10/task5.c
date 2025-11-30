#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// printers
#define K 3  
// jobs    
#define N 10     

sem_t printers;           
pthread_mutex_t count_mtx;
int working_printers = 0;

void* print_job(void* arg) {
    int id = (long)arg;

    sem_wait(&printers);

    pthread_mutex_lock(&count_mtx);
    working_printers++;
    printf("Thread %d is printing. Number of working printers = %d\n", id, working_printers);
    pthread_mutex_unlock(&count_mtx);

    usleep(200000); 

    pthread_mutex_lock(&count_mtx);
    working_printers--;
    printf("Thread %d finished printing. Number of working printers = %d\n", id, working_printers);
    pthread_mutex_unlock(&count_mtx);

    sem_post(&printers);  

    return NULL;
}

int main() {
    pthread_t threads[N];

    sem_init(&printers, 0, K);               
    pthread_mutex_init(&count_mtx, NULL);

    for (int i = 0; i < N; i++) {
        if(pthread_create(&threads[i], NULL, print_job, (void*)(long)i)!=0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    for (int i = 0; i < N; i++) {
        if(pthread_join(threads[i], NULL)!=0) {
            perror("Failed to join thread");
            return 1;
        }
    }

    sem_destroy(&printers);
    pthread_mutex_destroy(&count_mtx);

    return 0;
}

