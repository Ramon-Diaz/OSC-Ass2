#ifndef prodcon
#define prodcon

#define MAXINSTRUCTIONS 1000 // define the maximum number of instructions from the instructions.txt file

int nthreads; // initialize the global variable to save the number of threads
char *linebuffer; // initialize the linebuffer for the getline
size_t buffsize = 20; // initialize the size of each line to 20 to get up to 9 digit instructions "T100\n"
char **args; // initialize the array, where the instructions will be saved.

// what does it mean ..Be careful about boundary conditions – full or empty conditions.
// same for the global variable where the number of jobs by each trade is collected
//int nthreads_jobs[100];
int *nthreads_jobs;

int *queue;

sem_t empty;
sem_t full;
int producer_index;
int consumer_index;

pthread_mutex_t mutex;
pthread_mutex_t filemutex;
pthread_mutex_t nthreadmutex;
FILE *fp; // create the log file

char line[50]; // a single line could hold up to 49 characters plus the enter
int work_command = 0;
int ask_command = 0;
int receive_command = 0;
int complete_command = 0;
int sleep_command = 0;

clock_t t; // to keep track of time
double time_taken;
double time_taken_to_complete[MAXINSTRUCTIONS];

#endif
