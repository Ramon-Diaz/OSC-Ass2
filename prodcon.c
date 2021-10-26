#include <stdio.h> // to handle printf
#include <stdlib.h> // to handle system arguments
#include <string.h> // to handle strcpy
#include <pthread.h> // to handle threads
#include <semaphore.h> // to handle semaphores

#include "prodcon.h"
#include "tands.h"

void *prod(void *producer_thread_id){
    
    printf("Producer will begin...\n");
    t = clock();
    // begin an index for the number of instructions
    int i = 0;
    // while there are instructions available, loop
    while( args[i] != NULL){

        // if the instructions contain S then call sleep function
        if( strstr(args[i],"S") != NULL ){
            printf("Producer will sleep for: %d\n", atoi(args[i]+1));
            pthread_mutex_lock(&filemutex);
            t = clock() - t;
            time_taken = ((double)t)/CLOCKS_PER_SEC;
            snprintf(line, sizeof(line), "%f  ID= 0          Sleep        %d\n", time_taken, atoi(args[i]+1));
            fputs(line , fp);
            pthread_mutex_unlock(&filemutex);
            // convert the string to integer and pass it to Sleep function
            Sleep(atoi(args[i]+1));
            sleep_command++;
            printf("Producer finished sleeping.\n");
        } else {
            // wait for the queue not full
            sem_wait(&empty);
            // wait to be the only one accesing the queue to write a job
            pthread_mutex_lock(&mutex);

            // write in the queue
            queue[producer_index] = atoi(args[i]+1);
            printf("Producer put a job of: T%d in queue slot %d\n",atoi(args[i]+1), producer_index);
            // go to the next queue element
            producer_index = (producer_index+1)%QUEUESIZE;
            work_command++;

            // free the access to the queue
            pthread_mutex_unlock(&mutex);
            // send signal queue is not empty
            sem_post(&full);

            pthread_mutex_lock(&filemutex);
            t = clock() - t;
            time_taken = ((double)t)/CLOCKS_PER_SEC;
            snprintf(line, sizeof(line), "%f  ID= 0   Q= %d   Work         %d\n", time_taken, producer_index-1, atoi(args[i]+1));
            fputs(line , fp);
            pthread_mutex_unlock(&filemutex);
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
        queue[producer_index] = -1;
        printf("Producer put a termination job.\n");
        // go to the next queue element
        producer_index = (producer_index+1)%QUEUESIZE;

        // free the access to the queue
        pthread_mutex_unlock(&mutex);
        // send signal queue is not empty
        sem_post(&full);
    }
    printf("Producer: Finished, waiting for consumers to finish...\n");
    return NULL;   
}

void *cons(void *cno){
    // initialize thread counter for each job taken
    int nthread_taken_jobs = 0;
    while(1){
        // write in log that thread is asking for job
        pthread_mutex_lock(&filemutex);
        //char line[50];
        t = clock() - t;
        time_taken = ((double)t)/CLOCKS_PER_SEC;
        snprintf(line, sizeof(line), "%f  ID= %d          Ask\n", time_taken, *((int *)cno));
        fputs(line , fp);
        ask_command++;
        pthread_mutex_unlock(&filemutex);
        // wait for the queue not empty, until it has data.
        sem_wait(&full);
        // only one thread can access the queue at a time
        pthread_mutex_lock(&mutex);

        // get a job from the queue
        int item = queue[consumer_index];
        // go to the next queue element
        consumer_index = (consumer_index+1)%QUEUESIZE;
        // we free the access to the queue
        pthread_mutex_unlock(&mutex);
        // send signal that queue is not full
        sem_post(&empty);

        if (item != -1){
        pthread_mutex_lock(&filemutex);
        //char line[50];
        t = clock() - t;
        time_taken = ((double)t)/CLOCKS_PER_SEC;
        snprintf(line, sizeof(line), "%f  ID= %d   Q= %d   Receive      %d\n", time_taken, *((int *)cno), consumer_index-1, item);
        fputs(line , fp);
        receive_command++;
        pthread_mutex_unlock(&filemutex);
        // we execute the job
        printf("Thread: %i grab job: T%d from the queue slot: %d\n", *((int *)cno), item, consumer_index-1);
        Trans(item);
        printf("Thread: %i finished job: %d\n", *((int *)cno), item);
        
        pthread_mutex_lock(&filemutex);
        //char line[50];
        t = clock() - t;
        time_taken_to_complete[complete_command] = ((double)t)/CLOCKS_PER_SEC;
        snprintf(line, sizeof(line), "%f  ID= %d          Complete     %d\n", time_taken_to_complete[complete_command], *((int *)cno), item);
        fputs(line , fp);
        complete_command++;
        pthread_mutex_unlock(&filemutex);
        } else {
            // save the number of jobs each one did only one at a time
            pthread_mutex_lock(&nthreadmutex);
            nthreads_jobs[*((int *)cno)-1] = nthread_taken_jobs;
            pthread_mutex_unlock(&nthreadmutex);
            printf("Consumer: %i finished with a total of %d jobs.\n", *((int *)cno), nthread_taken_jobs);
            return NULL;
        }
        nthread_taken_jobs++;
    }
    

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
        // save the second argument to nthreads variable
        nthreads = atoi(argv[1]);
        // if no id argument is supplied go to default (0) and open the log file as append
        if( argc == 2 ){
            fp = fopen("prodcon.log", "a+");
        } else {
            char file_name[50];
            // parse the name according the the argument pass in command
            snprintf(file_name, sizeof(file_name), "prodcon.%s.log", argv[2]);
            fp = fopen(file_name, "a+"); 
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
        // create an array with threads id's to pass to the consumer function
        int cons_id[nthreads];
        for (int i =0; i< nthreads; i++){
            cons_id[i] = i+1;
        }

        // begin the producer thread
        pthread_t producer_thread, consumer_thread[nthreads];
        // initialize the semaphores
        pthread_mutex_init(&mutex, NULL);
        pthread_mutex_init(&filemutex, NULL);
        pthread_mutex_init(&nthreadmutex, NULL);
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
        
        // print the summary to the log after all threads have finished
        fputs("Summary:\n", fp);
        snprintf(line, sizeof(line), "    Work:         %d\n",work_command);
        fputs(line,fp);
        snprintf(line, sizeof(line), "    Ask:          %d\n",ask_command);
        fputs(line,fp);
        snprintf(line, sizeof(line), "    Receive:      %d\n",receive_command);
        fputs(line,fp);
        snprintf(line, sizeof(line), "    Complete:     %d\n",complete_command);
        fputs(line,fp);
        snprintf(line, sizeof(line), "    Sleep:        %d\n",sleep_command);
        fputs(line,fp);
        for(int i=0; i<nthreads; i++){
            snprintf(line,sizeof(line), "    Thread  %d     %d\n", i+1, nthreads_jobs[i]);
            fputs(line,fp);
        }
        
        // get the max time took to complete the last task and put it at the begining of the array
        for (int i = 1; i < MAXSLOTSJOBS; ++i) {
            if (time_taken_to_complete[0] < time_taken_to_complete[i]) {
                time_taken_to_complete[0] = time_taken_to_complete[i];
            }
        }
        // print the number of transaction per second according to the total time took to complete the last job
        snprintf(line, sizeof(line), "Transactions per second: %f in %f", work_command/time_taken_to_complete[0], time_taken_to_complete[0]);
        fputs(line,fp);
        
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
    pthread_mutex_destroy(&filemutex);
    pthread_mutex_destroy(&nthreadmutex);
    sem_destroy(&empty);
    sem_destroy(&full);
    // close the log file
    fclose(fp);
    // free the memory space of the dynamic memory space
    free(args);

    return 0;
}
