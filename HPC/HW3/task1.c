#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define P 10
#define R 2

pthread_barrier_t barrier;
int scores[P][R];
int wins[P] = {0};

void * roll_dice(void * arg) {
    int player_index = (int)(long)arg;
    int max = 0;
    int num;

    for (int round = 0; round < R; round++)  {
         num = (rand() % 6 + 1);
         scores[player_index][round] = num;
         pthread_barrier_wait(&barrier);

        if(player_index == 0) {
            max = 0;
            for(int p = 0; p < P; p++) {
                if( scores[p][round] >scores[max][round]) {
                    max = p;  
                }
            }
            wins[max]++;
            for (int p = 0; p < P; p++) {
                printf("Round %d: Player %d rolled %d\n", round + 1, p + 1, scores[p][round]);
            }

            printf("Round %d winner: Player %d (rolled %d)\n", round + 1, max + 1, scores[max][round]);
        }

        pthread_barrier_wait(&barrier);
    }
    return NULL;

}

int main() {
    pthread_barrier_init(&barrier, NULL, P);
    pthread_t threads[P];

    for (int j = 0; j < P; j++) {
        if (pthread_create(&threads[j], NULL, roll_dice, (void*)(long)j) != 0) {
            perror("Thread creation failed\n");
            for (int p = 0; p < j; p++){
                pthread_join(threads[p], NULL);
            }
            return 1;
        }
    }
    for (int i = 0; i < P; i++) {
        if(pthread_join(threads[i], NULL) != 0) {
            perror("Thread join failed\n");
            return 1;
        }
    }

    int max = 0;
    for(int i = 0; i < P; i++) {
        if(wins[i] > wins[max]) {
            max = i;
        }
    }

    printf("Player %d is the winner with a total wins of %d\n", max + 1, wins[max]);

    pthread_barrier_destroy(&barrier);

    return 0;

}
