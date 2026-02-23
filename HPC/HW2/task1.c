#include <stdio.h>
#include <pthread.h>

#define NUM 3

void* print_id(void*arg)
 {
    pthread_t tid = pthread_self();
    printf("Thread with ID: %lu is running\n", (unsigned long)tid);
    return NULL;
 }
int main() {

    pthread_t tids[NUM];


    for(int i = 0; i < NUM; i++) {
        pthread_t tid;
        if (pthread_create(&tids[i], NULL, print_id, NULL) != 0) {
            perror("Failed to create thread");
        }
    }

    for(int i = 0; i < NUM; i++) {
        pthread_join(tids[i], NULL);
    }


}
