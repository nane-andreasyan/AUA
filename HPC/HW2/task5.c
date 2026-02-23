#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <sched.h>

#define NUM 3

void* func(void*arg)
 {
    int cpu_num = sched_getcpu();
    pthread_t tid = pthread_self();
    int num = 0;
    while(num<500000000) {
        num++;
    }
    printf("Thread with ID: %lu is running on CPU: %d\n", (unsigned long)tid, cpu_num);
    return NULL;
 }
int main() {

    pthread_t tids[NUM];


    for(int i = 0; i < NUM; i++) {
        pthread_t tid;
        if (pthread_create(&tids[i], NULL, func, NULL) != 0) {
            perror("Failed to create thread");
        }
    }

    for(int i = 0; i < NUM; i++) {
        pthread_join(tids[i], NULL);
    }

}
