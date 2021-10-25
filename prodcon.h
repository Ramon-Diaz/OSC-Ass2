#ifndef prodcon
#define prodcon

#define MAXINSTRUCTIONS 50 // define the maximum number of instructions from the instructions.txt file

int nthreads; // initialize the global variable to save the number of threads
int file_id; // initialize the global variable to save the id name
char *linebuffer; // initialize the linebuffer for the getline
size_t buffsize = 5; // initialize the size of each line to 3 as we only have 3 characters "T2\n"
char **args; // initialize the array, where the instructions will be saved.


#endif
