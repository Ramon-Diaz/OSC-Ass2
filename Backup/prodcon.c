#include <stdio.h> // to handle printf
#include <stdlib.h> // to handle system arguments
#include <string.h> // to handle strcpy
#include <pthread.h> // to handle threads
#include <semaphore.h> // to handle semaphores

#include <signal.h> // to handle signal to be eliminated

#include "prodcon.h"
#include "tands.h"

#define QUEUESIZE 10
sem_t empty;
sem_t full;
int in;
int out;
int queue[QUEUESIZE];
pthread_mutex_t mutex;

void *prod(void *producer_thread_id){
    
    printf("Producer will begin...\n");
    // begin an index for the number of instructions
    int i = 0;
    // while there are instructions available, loop
    while( args[i] != NULL){

        // if the instructions contain S then call sleep function
        if( strstr(args[i],"S") != NULL ){
            printf("Producer will sleep for: %d\n", atoi(args[i]+1));
            // convert the string to integer and pass it to Sleep function
            Sleep(atoi(args[i]+1));
            printf("Producer finished sleeping.\n");
        } else {
            // wait for the queue not full
            sem_wait(&empty);
            // wait to be the only one accesing the queue to write a job
            pthread_mutex_lock(&mutex);

            // write in the queue
            queue[in] = atoi(args[i]+1);
            printf("Producer put a job of: T%d in queue slot %d\n",atoi(args[i]+1), in);
            // go to the next queue element
            in = (in+1)%QUEUESIZE;

            // free the access to the queue
            pthread_mutex_unlock(&mutex);
            // send signal queue is not empty
            sem_post(&full);
        }
        // increase the index of instructions
        i++;
    }
    // once the producer has finished posting jobs in the queue we will
    for(int i=0; i<nthreads;i++){
        // wait for the queue not full
        sem_wait(&empty);
        // wait to be the only one accesing the queue to write a job
        pthread_mutex_lock(&mutex);
        
        // write in the queue
        queue[in] = -1;
        printf("Producer put a termination job.\n");
        // go to the next queue element
        in = (in+1)%QUEUESIZE;

        // free the access to the queue
        pthread_mutex_unlock(&mutex);
        // send signal queue is not empty
        sem_post(&full);
    }
    printf("Producer: Finished, waiting for consumers to finish...\n");
    return NULL;   
}

void *cons(void *cno){
    
    while(1){
        // wait for the queue not empty, until it has data.
        sem_wait(&full);
        // only one thread can access the queue at a time
        pthread_mutex_lock(&mutex);

        // get a job from the queue
        int item = queue[out];
        // go to the next queue element
        out = (out+1)%QUEUESIZE;
        // we free the access to the queue
        pthread_mutex_unlock(&mutex);
        // send signal that queue is not full
        sem_post(&empty);

        if (item != -1){
        // we execute the job
        printf("Thread: %i grab job: T%d from the queue slot: %d\n", *((int *)cno), item, out-1);
        Trans(item);
        printf("Thread: %i finished job: %d\n", *((int *)cno), item);
        } else {
            return NULL;
        }
    }
    printf("Consumer: %i finished.\n", *((int *)cno));
    return NULL;
}

int main(int argc, char *argv[]){
    /* The main function receives the system arguments
    Input:
        - argc: numerical number of arguments
        - argv: the char system arguments
    */ 
    
    // allocate dynamic memory for the number of instructions
    args = malloc(MAXINSTRUCTIONS*sizeof(char*));
    // check the amount of arguments provided to the terminal
    // if the number of provided arguments is two save them to the variable
    if( argc == 3 || argc == 2 ){
        nthreads = atoi(argv[1]);
        // if no id argument is supplied go to default (0)
        if( argc == 2 ){
            file_id = 0;
        } else {
            file_id = atoi(argv[2]);
        }
        // initialize the counter index for the args array
        int i = 0;
        while( getline(&linebuffer, &buffsize, stdin) != -1 ){
            // allocate space for each separate line
            args[i] = malloc(strlen(linebuffer)+1);
            // eliminate the \n at the end of the fgets
            linebuffer[strcspn(linebuffer, "\n")] = 0;
            // copy the line to the dynamic array
            strcpy(args[i], linebuffer);
            // increment the index by one
            i++;
        }
        // create an array with threads id's
        int cons_id[nthreads];
        for (int i =0; i< nthreads; i++){
            cons_id[i] = i+1;
        }

        // begin the producer thread
        pthread_t producer_thread, consumer_thread[nthreads];
        pthread_mutex_init(&mutex, NULL);
        sem_init(&empty,0, QUEUESIZE);
        sem_init(&full,0,0);

        // create the producer thread
        pthread_create(&producer_thread, NULL, prod, NULL);
        
        // create the consumers threads
        for(int id_thread = 0; id_thread < nthreads ; id_thread++){
            pthread_create(&consumer_thread[id_thread], NULL, cons, (void *)&cons_id[id_thread]);
        }
        // wait for the producer thread to finish and all of its consumers
        pthread_join(producer_thread, NULL);
        for(int id_thread = 0; id_thread < nthreads; id_thread++){
            pthread_join(consumer_thread[id_thread], NULL);
        }
        printf("Main: all threads finished.\n");
        
    // if more than 2 arguments are supplied exit
    } else if( argc > 3 ) {
        printf("Too many arguments supplied.\n");
    }
    // if no arguments are supplied then exit
    else{
        printf("Error: At least one argument expected.\n");
    }
    // perform some cleaning by eliminating the semaphores
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
    // free the memory space of the dynamic memory space
    free(args);

    return 0;
}
