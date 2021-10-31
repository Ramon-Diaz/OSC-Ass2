#include <pthread.h>

#include "timer_.h"

double get_time(clock_t timer){
    /* function that returns the step of the input timer
    Input:
        - clock_t timer: clock variable that stores the number of transcurred clocks
    Output:
        - it returns the time in seconds transcurred since initialization of the timer
    */
    // take another step in the timer
    timer = clock();
    // get the time in seconds, just by dividing over the amount of clocks per second
    return ((double)timer)/CLOCKS_PER_SEC;
}