//
// Created by student on 5/31/20.
//

#include "Barrier.h"

void Barrier::wait() {
    sem_wait(&mutex);
    countFirst++;
    if (countFirst == n) {
        sem_post(&firstPhase);
    }
    sem_post(&mutex);

    sem_wait(&firstPhase);
    sem_post(&firstPhase);

    sem_wait(&mutex);
    countFirst--;
    if (countFirst == 0) {
        sem_wait(&firstPhase);
    }
    sem_post(&mutex);

    sem_wait(&mutex);
    countSecond++;
    if (countSecond == n) {
        sem_post(&secondPhase);
    }
    sem_post(&mutex);
    sem_wait(&secondPhase);
    sem_post(&secondPhase);

    sem_wait(&mutex);
    countSecond--;
    if(countSecond == 0){
        sem_wait(&secondPhase);
    }
    sem_post(&mutex);
}
