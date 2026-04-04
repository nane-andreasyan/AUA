#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

typedef enum {
    HIGH,
    NORMAL
} Priority;


typedef struct {
    int order_id;
    int distance_km;
    Priority priority;
} DeliveryOrder;

#define NUM_THREADS 4

int main() {
    DeliveryOrder orders[10000];
    omp_set_num_threads(NUM_THREADS);
    int thread_high_count[NUM_THREADS] = {0};
    int threshold;

    #pragma omp parallel 
    {
        #pragma omp single 
        {   
            threshold = 20;
            for(int i = 0; i < 10000; i++) {
                orders[i].order_id = i;
                orders[i].distance_km = rand() % 100;
            }
        }

        #pragma omp barrier

        #pragma omp for
        for(int i = 0; i < 10000; i++) {
            if (orders[i].distance_km < threshold) {
                orders[i].priority = HIGH;
            } else {
                orders[i].priority = NORMAL;
            }
        }
        
        #pragma omp barrier

        #pragma omp single 
        {
            printf("Priority assignment is finished.\n");
        }

        #pragma omp for
        for(int i = 0; i < 10000; i++) {
            int thread_id = omp_get_thread_num();
            if (orders[i].priority == HIGH ) {
                thread_high_count[thread_id]++;
            }
        }

        #pragma omp barrier

        #pragma omp single 
        {
            int total=0;
            for(int i = 0; i < NUM_THREADS; i++) {
                total += thread_high_count[i];
                printf("Thread %d high priority orders: %d\n", i, thread_high_count[i]);
            }

            printf("Total high priority orders: %d\n", total);
        }

    }

}