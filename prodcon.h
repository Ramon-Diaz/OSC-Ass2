#ifndef prodcon
#define prodcon

#define MAXINSTRUCTIONS 50 // define the maximum number of instructions from the instructions.txt file

int nthreads; // initialize the global variable to save the number of threads
char *linebuffer; // initialize the linebuffer for the getline
size_t buffsize = 5; // initialize the size of each line to 5 as we only could have up to 5 characters "T100\n"
char **args; // initialize the array, where the instructions will be saved.

// how to define the size of the queue?? 2* argument 1 of system input as a global variable
// what does it mean ..Be careful about boundary conditions – full or empty conditions.
// same for the global variable where the number of jobs by each trade is collected
int nthreads_jobs[30];
// how to get the time
// do we also need to have that space, how is that possible?

#define QUEUESIZE 10
sem_t empty;
sem_t full;
int producer_index;
int consumer_index;
int queue[QUEUESIZE];
pthread_mutex_t mutex;
pthread_mutex_t filemutex;
pthread_mutex_t nthreadmutex;
FILE *fp; // create the log file

char line[50];
int work_command = 0;
int ask_command = 0;
int receive_command = 0;
int complete_command = 0;
int sleep_command = 0;

clock_t t; // to keep track of time
double time_taken;
#define MAXSLOTSJOBS 100
double time_taken_to_complete[MAXSLOTSJOBS];

#endif
