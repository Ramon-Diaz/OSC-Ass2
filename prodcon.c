#include <stdio.h> // to handle printf
#include <stdlib.h> // to handle system arguments
#include <string.h> // to handle strcpy
#include <pthread.h> // to handle threads
#include <semaphore.h> // to handle semaphores

#include "prodcon.h"
#include "tands.h"

#define BUFFERSIZE 10
// sem_t empty;
// sem_t full;
int in = 0;
// int out = 0;
int buffer[BUFFERSIZE];
// pthread_mutex_t mutex;

void *prod(void *pno){
    
    printf("Producer will begin...\n");
    // begin an index for instructions
    int i = 0;
    // while there are instructions available, loop
    while( args[i] != NULL){
        printf("%s\n",args[i]);
        // if the instructions contain S then call sleep function
        if( strstr(args[i],"S") != NULL ){
            printf("Producer will sleep for: %d\n", atoi(args[i]+1));
            // convert the string to integer and pass it to Sleep function
            Sleep(atoi(args[i]+1));
            printf("Producer finished sleeping.\n");
        } else {
            buffer[in] = atoi(args[i]+1);
            in = (in+1)%BUFFERSIZE;
            printf("Producer put a T job of: %d\n",atoi(args[i]+1));
        }
        
        // increase the index of instructions
        i++;
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

        // begin the producer thread
        pthread_t producer_thread;
        //pthread_mutex_init(&mutex, NULL);
        //sem_init(&empty,0, BUFFERSIZE);
        //sem_init(&full,0,0);

        pthread_create(&producer_thread, NULL, prod, NULL);

        pthread_join(producer_thread, NULL);
        printf("Thread finished...\n");
    
        //pthread_mutex_destroy(&mutex);
        //sem_destroy(&empty);
        //sem_destroy(&full);
        

    // if more than 2 arguments are supplied exit
    } else if( argc > 3 ) {
        printf("Too many arguments supplied.\n");
    
    }
    // if no arguments are supplied then exit
    else{
        printf("Error: At least one argument expected.\n");
        
    }
    // free the memory space of the dynamic memory space
    free(args);
    return 0;
}
