#ifndef prodcon
#define prodcon

#define MAXINSTRUCTIONS 100000 // define the maximum number of instructions from the instructions.txt file

int nthreads; // initialize the global variable to save the number of threads
char *linebuffer; // initialize the linebuffer for the getline
size_t buffsize = 10; // initialize the size of each line to 10 to get up to 9 digit instructions "T100000000\n"
char **args; // initialize the array, where the instructions will be saved.

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

char line[50]; // a single line could hold up to 49 characters plus the enter to print in the log.
int work_command = 0;
int ask_command = 0;
int receive_command = 0;
int complete_command = 0;
int sleep_command = 0;
int n_jobs_in_queue = 0;

clock_t timer; // to keep track of time
double time_taken;
double time_taken_to_complete[MAXINSTRUCTIONS];

#endif
