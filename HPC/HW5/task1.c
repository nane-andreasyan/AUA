#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


typedef struct {
    int request_id;
    int user_id;
    int response_time_ms;
} LogEntry;

#define NUM_THREADS 4

int main() {
    LogEntry logs[20000];
    omp_set_num_threads(NUM_THREADS);

    int fast_counts[NUM_THREADS] = {0};  
    int medium_counts[NUM_THREADS] = {0};
    int slow_counts[NUM_THREADS] = {0};

    #pragma omp parallel 
    {
        #pragma omp single 
        {
            for(int i = 0; i < 20000; i++) {
                logs[i].request_id = i;
                logs[i].user_id = i % 100;
                logs[i].response_time_ms = rand() % 1000;
            }
        }

        #pragma omp barrier

        #pragma omp for 
        for(int i = 0; i < 20000; i++) {
            int thread_id = omp_get_thread_num();
            if(logs[i].response_time_ms < 100) {
                fast_counts[thread_id]++;
            } else if(logs[i].response_time_ms <= 300) {
                medium_counts[thread_id]++;
            } else {
                slow_counts[thread_id]++;
            }
        }
        
        #pragma omp barrier


        #pragma omp single 
        {
            int fast = 0, medium = 0, slow = 0;
            for(int i = 0; i < NUM_THREADS; i++) {
                fast += fast_counts[i];
                medium += medium_counts[i];
                slow += slow_counts[i];
            }
            printf("Fast requests: %d\n", fast);
            printf("Medium requests: %d\n", medium);
            printf("Slow requests: %d\n", slow);
        }

    }
}