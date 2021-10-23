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

    // check the amount of arguments provided to the terminal
    // if the number of provided arguments is two save them to the variable
    printf("%i\n", argc);
    if(argc == 3){
        nthreads = atoi(argv[1]);
        id = atoi(argv[2]);
        while (fgets (filebuffer, BUFFSIZE, stdin))
            fputs (filebuffer, stdout);
    // if no id argument is supplied go to default (0)
    } else if(argc == 2){
        nthreads = atoi(argv[1]);
        id = 0;
    // if more than 2 arguments are supplied exit
    } else if( argc > 5 ) {
        printf("Error: Too many arguments supplied.\n");
        exit(1);
    }
    // if no arguments are supplied then exit
    else {
        printf("Error: At least one argument expected.\n");
        exit(1);
    }

    return 0;
}
