#include <stdio.h> // to handle printf
#include <stdlib.h> // to handle system arguments
#include <string.h> // to handle strcpy

#include "prodcon.h"

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
            id = 0;
        } else {
            id = atoi(argv[2]);
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
    // if more than 2 arguments are supplied exit
    } else if( argc > 3 ) {
        printf("Error: Too many arguments supplied.\n");
        exit(1);
    }
    // if no arguments are supplied then exit
    else{
        printf("Error: At least one argument expected.\n");
        exit(1);
    }
    // free the memory space of the dynamic memory space
    free(args);
    return 0;
}
