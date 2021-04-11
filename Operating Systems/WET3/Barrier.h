//
// Created by student on 5/31/20.
#ifndef BARRIER_H_
#define BARRIER_H_
#include <iostream>
#include <semaphore.h>
using namespace std;

class Barrier {
public:
    Barrier(unsigned int num_of_threads):
            countFirst(0),countSecond(0), n(num_of_threads){

        sem_init(&mutex, 0, 1);
        sem_init(&firstPhase, 0, 0);
        sem_init(&secondPhase, 0, 0);
    }
    void wait() ;
    ~Barrier(){
        sem_destroy(&mutex);
        sem_destroy(&firstPhase);
        sem_destroy(&secondPhase);

    }
    int waitingThreads(){
        int countFir = 0;
        sem_wait(&mutex);
        countFir = countFirst;
        sem_post(&mutex);
        return  countFir;
    }
    int countFirst;
    int countSecond;
    int n;
    sem_t mutex;
    sem_t firstPhase  ;
    sem_t secondPhase ;

    // TODO: define the member variables
    // Remember: you can only use semaphores!
};


/*buy_popcorn();
buy_something_to_drink();
// wait for everyone else before we start the movie
barrier.wait();
watch_the_movie();
// wait until everyone leaves the cinema
barrier.wait();
close_the_door();
*/




#endif // BARRIER_H_