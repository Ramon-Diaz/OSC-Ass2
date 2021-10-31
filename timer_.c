#include <pthread.h>

#include "timer_.h"

double get_time(clock_t timer){
    // take another step in the timer
    timer = clock();
    // get the time in seconds, just by dividing over the amount of clocks per second
    return ((double)timer)/CLOCKS_PER_SEC;
}