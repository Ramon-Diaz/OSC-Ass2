# Operating System Concepts - Assignment 2

## Author

Ramon Diaz Ramos <br />
CCID: diazramo <br />
Date: October 30, 2021 <br />

<details open="open">
<summary>Table of Contents</summary>

- [Files](#folders)
- [Getting Started](#getting-started)
  - [Execution](#usage)
- [Assumptions](#assumptions)
- [Approach](#approach)
- [Bibliography](#bibliography)

</details>

## Files

- Makefile: contains the set of instructions to compile the program
- prodcon.c: is the main program to be executed. It contains the main, where the instructions are saved and executed. It also includes the producer and consumer functions which are executed in threads. Additionally, it includes all functions that write in the log file at the beginning of the program.
- prodcon.h: the header file which contains all global variables that are used by the prodcon c file
- tands.c: provided file to simulate work of the producer and the consumers
- tands.h: header file that contains both functions (Trans, Sleep) that are in the tands.c file
- timer_.c: it contains the function get_time(), which returns a step of the initialized timer
- timer_.h: the header file of the timer_,c file

## Getting Started

### Execution

Use the following command to unzip the folder in Linux:

    unzip ass2_RamonDiaz.zip

Then executing the following command will compile and link the files:

    make

After compiling all files, you can execute the code as follows:

    ./prodcon 3 1 <instructions

Where _./prodcon_ indicates the file to execute, the following command is the number of threads (must be included), the third argument is the file's name (optional), and at last, you can input a file with the instructions to execute by the program.

In case that the instructions are going to be provided in the terminal, you can execute it as follows:

    ./prodon 3 1

Then, the program will be idle until the instructions are provided manually separated by an end of line (to finish, just press Ctrl-D):

    T1
    T2
    T4
    S9
    T1
    <Ctrl-D>

## Assumptions

My program has the following assumptions:

1. The maximum set of instructions that can be provided are 99,999. Varibale `MAXINSTRUCTIONS` captures that value. IF more instructions are input then the variable must be modified.
2. Each instructions can be composed of nine digits numbers, no more than that. Variable `buffsize` has that definition.
3. Each printed line in the log file can contain up to 49 characters. Variable `line` has that definition. It can be modified to capture more characters in each line of the log file.
4. If less than 2 or more than 3 arguments are provided (e.g., ./prodcon (blank), ./prodcon 3 1 1) it will exit with an error.

## Approach

* A brief summary of the approach you took to solving the assignment.

The general idea of my program was to deploy two types of threads: producer and as many consumers as needed. The producer thread receives the instructions from the main function and starts executing them one by one. After the producer is created, the main program creates as many consumers as provided by the user. Every time a thread wanted to write/read on the queue, it must pass two semaphores. The first semaphore lock and unlock if the queue is empty or full, and the second semaphores locks and unlock the critical section where the thread can access the queue. Once a consumer takes a job, it increases the shared variable that captures the consumers' index of jobs inside the critical section. Once a producer puts a job in the queue, its index is increased by one. The program finishes once all jobs have been put in the queue by the producer and once the consumers complete all jobs. The producer will get out of the loop and write termination jobs in the queue for the consumers to read them and break out of the infinite loop.

My program has an additional binary semaphore that allows only one thread to write in the log file and another to modify the number of jobs that the consumers have completed.

## Bibliography

I discussed the main function with Zijian Zhou. We didn't exchange the code.

I based my idea to construct the semaphores on the following medium post: https://medium.com/@sohamshah456/producer-consumer-programming-with-c-d0d47b8f103f

I obtained the pthreads functions from the labs.