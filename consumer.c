#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h> 
#include <pthread.h>
#include "helpers.h"

//Consumer Process that runs the Distributor and Worker Threads. Returns exit status
int main(int argc, char *argv[]){ 

    //Initializing product storage buffers, and initializing all positions to -2 (empty)
    int buff1[20];
    for (int i = 0; i < 20; ++i){
        buff1[i] = -2;
    }

    int buff2[10];
    for (int i = 0; i < 10; ++i){
        buff2[i] = -2;
    }

    //Creating pthread ids, argument structs
    pthread_t distr;
    pthread_t work1_1;
    pthread_t work1_2;
    pthread_t work2_1;
    pthread_t work2_2;
    myarg_dist args;
    myarg_buff buffer1;
    myarg_buff buffer2;

    //Creates Output File, and sets up Column Format
    FILE *output;
    output = fopen("./output.txt", "w+");
    fprintf(output, "%s\t%s\t%s\t%s\n", "Product Type", "Consumption Thread ID", 
    "Production Sequence Number", "Consumption Sequence Number");

    //Initializing queue head, queue tails, sizes, and consumption counts to 0
    buffer1.head = 0; buffer1.tail = 0; 
    buffer2.head = 0; buffer2.tail = 0;
    buffer1.filled = 0; buffer2.filled = 0;
    buffer1.consumed = 0; buffer2.consumed = 0; 

    //Initializing queue sizes and iteration control variables
    buffer1.size = 20; buffer2.size = 10;
    buffer1.isActive = 1; buffer2.isActive = 1;

    //initializing Mutexes and Condition Variables
    pthread_mutex_init(&buffer1.mutex, NULL); pthread_mutex_init(&buffer2.mutex, NULL);
    pthread_cond_init(&buffer1.fill, NULL); pthread_cond_init(&buffer2.fill, NULL);
    pthread_cond_init(&buffer1.empty, NULL); pthread_cond_init(&buffer2.empty, NULL);

    //Assigning output file and allocating space for the queues
    buffer1.output = output; buffer2.output = output;
    buffer1.buffer = malloc(buffer1.size * sizeof(int));
    buffer2.buffer = malloc(buffer2.size * sizeof(int));

    //Copying pipe data and buffer data to respective structs
    memcpy(&buffer1.buffer[0], &buff1[0], sizeof(int) * 20);
    memcpy(&buffer2.buffer[0], &buff2[0], sizeof(int) * 10);

    //Copying buffer data to args
    args.buffer1 = &buffer1;
    args.buffer2 = &buffer2;

    //Running Distributor Thread
    pthread_create(&distr, NULL, distributor, &args);
    pthread_create(&work1_1, NULL, worker, &buffer1);
    pthread_create(&work2_1, NULL, worker, &buffer2);
    pthread_create(&work1_2, NULL, worker, &buffer1);
    pthread_create(&work2_2, NULL, worker, &buffer2);

    //Joining all Threads
    pthread_join(work1_1, NULL);
    pthread_join(work2_1, NULL);
    pthread_join(work1_2, NULL);
    pthread_join(work2_2, NULL);
    pthread_join(distr, NULL);    
    
    //Closing output file
    fclose(output);
    exit(0);
}