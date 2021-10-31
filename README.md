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

- Makefile: contains the set of intructions to compile the program
- prodcon.c: is the main program to be executed, it contains the main, where the instructions are save and executed. It also contains the producer and consumer functions which are executed in threads. Additionally it contains all functions that write in the log file at the begining of the program.
- prodcon.h: the header file which contains all global variables that are used by the prodcon c file
- tands.c: provided file to simulate work of the producer and the consumers
- tands.h: header file that contain both functions (Trans, Sleep) externed by the tands.c file
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

* List any assumptions you made in doing your assignment. It's easier if you tell us, rather than us having to find out the hard way.



## Approach

* A brief summary of the approach you took to solving the assignment.

## Bibliography

