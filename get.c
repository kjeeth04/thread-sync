#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h> 
#include <time.h>
#include <pthread.h>
#include "helpers.h"

/*  Function for getting product from circular queue
        @param buffer : buffer from which product will be removed
        @param tail : position within buffer from which product will be removed
        @param size : size of buffer
        @param consumed : consumption count for that buffer
        @param isActive : iteration boolean for worker loop
        @param empty : condition variable to send signal
        @param output : File to which transaction data is written
*/
void get(int* buffer, int* tail, int* size, int* filled, int* consumed, int* isActive, pthread_cond_t* empty, FILE* output){
    //Value in buffer position is stored and position is set to -2 (empty)
    int val = buffer[*tail];
    buffer[*tail] = -2;

    //Makes iteration boolean false and sends signal if -1 is read
    if (val == -1){
        *isActive = 0;
        pthread_cond_signal(empty);
        *tail = (*tail + 1) % *size;
        return;
    }
    
    //Moves tail forward in circular queue
    *tail = (*tail + 1) % *size;

    //Identifies Product type based on the size of the buffer it was obtained from
    char productType[10];
    if (*size == 20){
        strcpy(&productType[0], "product_1");
    }
    else {
        strcpy(&productType[0], "product_2");
    }

    //Increments consumption count, decrements filled count, and outputs transaction data to output file
    ++(*consumed);
    --(*filled);
    fprintf(output,"%s\t%ld\t\t%d\t\t\t\t%d\n", productType, pthread_self(), val, *consumed);
}