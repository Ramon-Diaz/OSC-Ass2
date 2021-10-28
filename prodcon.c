#include <stdio.h>
#include <stdlib.h> // to handle system arguments
#include <string.h> // to handle strcpy
#include <pthread.h> // to handle threads
#include <semaphore.h> // to handle semaphores

#include "prodcon.h"
#include "tands.h"

void get_time(void){
    // take another step in the timer
    timer = clock();
    // get the time in seconds, just by dividing over the amount of clocks per second
    time_taken = ((double)timer)/CLOCKS_PER_SEC;
}

void write_sleep_in_file(int i){
    // only one thread can be writing at a time in the file fp
    pthread_mutex_lock(&filemutex);
    // get a time step
    get_time();
    // temporarily save the printed line to pass it to the file
    snprintf(line, sizeof(line), "%.3f  ID= 0          Sleep        %d\n", time_taken, atoi(args[i]+1));
    // put the printed line in the file
    fputs(line , fp);
    // increase the counter of sleep command
    sleep_command++;
    // free the mutex
    pthread_mutex_unlock(&filemutex);
}

void write_works_in_file(int i){
    pthread_mutex_lock(&filemutex);
    get_time();
    snprintf(line, sizeof(line), "%.3f  ID= 0   Q= %d   Work         %d\n", time_taken, n_jobs_in_queue, atoi(args[i]+1));
    fputs(line , fp);
    // increase the work command counter by one every time it put a job in the queue
    work_command++;
    pthread_mutex_unlock(&filemutex);
}

void write_end_in_file(void){
    pthread_mutex_lock(&filemutex);
    get_time();
    snprintf(line, sizeof(line), "%.3f  ID= 0          End\n", time_taken);
    fputs(line , fp);
    pthread_mutex_unlock(&filemutex);
}

void write_ask_in_file(int thread_identifier){
    pthread_mutex_lock(&filemutex);
    get_time();
    snprintf(line, sizeof(line), "%.3f  ID= %d          Ask\n", time_taken, thread_identifier);
    fputs(line , fp);
    // increase the ask command counter
    ask_command++;
    pthread_mutex_unlock(&filemutex);
}

void write_receive_in_file(int consumer_id, int item){
    pthread_mutex_lock(&filemutex);
    get_time();
    snprintf(line, sizeof(line), "%.3f  ID= %d   Q= %d   Receive      %d\n", time_taken, consumer_id, n_jobs_in_queue, item);
    fputs(line , fp);
    // increase the receive command counter after taking a job from the queue
    receive_command++;
    pthread_mutex_unlock(&filemutex);
}

void write_complete_in_file(int consumer_id, int item){
    pthread_mutex_lock(&filemutex);
    timer = clock();
    time_taken_to_complete[complete_command] = ((double)timer)/CLOCKS_PER_SEC;
    snprintf(line, sizeof(line), "%.3f  ID= %d          Complete     %d\n", time_taken_to_complete[complete_command], consumer_id, item);
    fputs(line , fp);
    complete_command++;
    pthread_mutex_unlock(&filemutex);
}

void *prod(void *producer_thread_id){
    // Producer will begin
    // We start the timer to take steps through the program of the time
    timer = clock();
    // begin an index for the number of instructions that we will be saved in args
    int i = 0;
    // while there are instructions available, loop
    while( args[i] != NULL){
        // if the instructions contain S then call sleep function
        if( strstr(args[i],"S") != NULL ){
            // write each sleep command in the queue
            write_sleep_in_file(i);
            // convert the string to integer and pass it to Sleep function
            Sleep(atoi(args[i]+1));
        } else {
            // if it is a transaction job then we will write in the queue when is not full and once at a time
            // wait for the queue not full

            sem_wait(&empty);
            // wait to be the only one accesing the queue to write a job
            pthread_mutex_lock(&mutex);
            // write in the queue
            queue[producer_index] = atoi(args[i]+1);
            // increase the counter of number of jobs in queue (q)
            n_jobs_in_queue++;
            // if the element in the queue is not a termination job then write it in the log
            if (queue[producer_index] != -1){
                // write each work produce in the queue
                write_works_in_file(i);
            }
            // set the index to the next queue element
            producer_index = (producer_index+1)%(2*nthreads);
            // free the access to the queue
            pthread_mutex_unlock(&mutex);
            // send signal queue is not empty
            sem_post(&full);

        }
        // increase the index to select the next instruction
        i++;
    }
    // once the producer has finished posting jobs in the queue we will write termination jobs for each thread
    for(int i=0; i<nthreads;i++){

        // wait for the queue not full
        sem_wait(&empty);
        // wait to be the only one accesing the queue to write a job
        pthread_mutex_lock(&mutex);
        // write in the queue
        queue[producer_index] = -1;
        // go to the next queue element
        producer_index = (producer_index+1)%(2*nthreads);
        // free the access to the queue
        pthread_mutex_unlock(&mutex);
        // send signal queue is not empty
        sem_post(&full);

    }
    // write the end of the producer in the log
    write_end_in_file();

    return NULL;   
}

void *cons(void *cno){
    // initialize thread counter for each job taken
    int nthread_taken_jobs = 0;
    while(1){
        // write in log that thread is asking for job
        write_ask_in_file(*((int *)cno));

        // wait for the queue not empty, until it has data.
        sem_wait(&full);
        // only one thread can access the queue at a time
        pthread_mutex_lock(&mutex);
        // get a job from the queue
        int item = queue[consumer_index];
        // decrease the number of jobs in queue after taking one.
        n_jobs_in_queue--;
        // write in the file and increase the receive counter only if it is not a termination job
        if (queue[consumer_index] != -1){
            write_receive_in_file(*((int *)cno), item);
        }
        // go to the next queue element
        consumer_index = (consumer_index+1)%(2*nthreads);
        // we free the access to the queue then we will execute the job to free the critical section
        pthread_mutex_unlock(&mutex);
        // send signal that queue is not full
        sem_post(&empty);


        if (item != -1){
            // we execute the job
            Trans(item);
            
            write_complete_in_file(*((int *)cno), item);
           
        } else {
            // save the number of jobs each one did only one at a time
            pthread_mutex_lock(&nthreadmutex);
            nthreads_jobs[*((int *)cno)-1] = nthread_taken_jobs;
            pthread_mutex_unlock(&nthreadmutex);
            printf("Consumer: %i finished with a total of %d jobs.\n", *((int *)cno), nthreads_jobs[*((int *)cno)-1]);
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
    if (args == NULL){
            printf("Memory error.\n");
            exit(1);
        }
    // check the amount of arguments provided to the terminal
    // if the number of provided arguments is two save them to the variable
    if( argc == 3 || argc == 2 ){
        // save the second argument to nthreads variable
        nthreads = atoi(argv[1]);
        // allocate dynamic memory for the queue size
        queue = malloc(2*nthreads*sizeof(int*));
        nthreads_jobs = malloc(nthreads*sizeof(int*));
        if (queue == NULL || nthreads_jobs == NULL){
            printf("Memory error.\n");
            exit(1);
        }
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
        sem_init(&empty,0, 2*nthreads);
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
        pthread_mutex_lock(&nthreadmutex);
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
        pthread_mutex_unlock(&nthreadmutex);
        
        // get the max time it takes to complete the last task and put it at the begining of the array
        for (int i = 1; i < MAXINSTRUCTIONS; ++i) {
            if (time_taken_to_complete[0] < time_taken_to_complete[i]) {
                time_taken_to_complete[0] = time_taken_to_complete[i];
            }
        }
        
        // print the number of transaction per second according to the total time took to complete the last job
        snprintf(line, sizeof(line), "Transactions per second: %f in %.3f\n", work_command/time_taken_to_complete[0], time_taken_to_complete[0]);
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
    // free the memory space of the dynamic memory spaces used
    free(args);
    free(queue);
    free(nthreads_jobs);

    return 0;
}
