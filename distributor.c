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

/*  Function to distribute products from pipe into respective buffer.
        @param arg : void* that will then get casted as a myarg_t, with contains both circular queues
        @return 0 always
*/
void *distributor(void *arg){
    
    //Casting arg as a myarg_t
    myarg_dist* args = (myarg_dist *) arg;

    //Booleans to check if Producer Processes 1 and 2 done
    int prod1active = 1;
    int prod2active = 1;

    //While either process is still active or there are still spaces left to be
    while (prod1active || prod2active){
        //Temp value to hold the product from the pipe
        int temp[2]; 
        //If pipe is empty, sleep for 0.1 seconds and then skip the rest of the loop
        if (read(0, temp, 8) == -1){
            struct timespec remaining, request = {0, 100000000}; 
            nanosleep(&request, &remaining);
            continue;
        }

        //If product came from Producer Process 1
        if (temp[1] == 1){
            //If the count is -1, then Producer Process 1 is complete, and the boolean will be adjusted to reflect this
            if (temp[0] == -1){
                prod1active = 0;
            }

            pthread_mutex_lock(&args->buffer1->mutex);

            //Waits for the buffer to have an empty space
            while (args->buffer1->filled == args->buffer1->size){
                pthread_cond_wait(&args->buffer1->empty, &args->buffer1->mutex);
            }

            //Moves product into buffer
            put(args->buffer1->buffer, temp[0], &args->buffer1->size, &args->buffer1->head, &args->buffer1->filled);

            printf("Product from 1 Received\n");
            //Signal to worker processes that there is a product in the buffer
            pthread_cond_signal(&args->buffer1->fill);
            pthread_mutex_unlock(&args->buffer1->mutex);
        }
        //If product came from Producer Process 2
        else if (temp[1] == 2){
            //If the count is -1, then Producer Process 2 is complete, and the boolean will be adjusted to reflect this
            if (temp[0] == -1){
                prod2active = 0;
            }

            //Moves product into buffer
            pthread_mutex_lock(&args->buffer2->mutex);

            //Waits for the buffer to have an empty space
            while (args->buffer2->filled == args->buffer2->size){
                pthread_cond_wait(&args->buffer2->empty, &args->buffer2->mutex);
            }

            put(args->buffer2->buffer, temp[0], &args->buffer2->size, &args->buffer2->head, &args->buffer2->filled);

            printf("Product from 2 Received\n");
            //Signal to worker processes that there is a product in the buffer
            pthread_cond_signal(&args->buffer2->fill);
            pthread_mutex_unlock(&args->buffer2->mutex);
        }

    }

    printf("Distributor Thread %ld Done\n", pthread_self());
    return 0;
}