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

/*  Function to remove and process product from buffer. No return type
    @param arg : void* that will then get casted as a myarg_t, with contains circular queue 1
    @return 0 always
*/
void *worker(void *arg){
    myarg_buff* buff = (myarg_buff *) arg;

    //Loop runs while there is still product to be consumed, and consumes product
    while (buff->isActive){

        pthread_mutex_lock(&buff->mutex);
        //Wait until there is a product in the buffer
        while(buff->filled == 0){
            pthread_cond_wait(&buff->fill, &buff->mutex);
        }

        if (buff->isActive){
            //Get Product from Buffer
            get(buff->buffer, &buff->tail, &buff->size, &buff->filled, &buff->consumed, &buff->isActive, &buff->empty,buff->output);

            if (buff->size == 20){
                printf("Product from 1 Processed: %d\n", buff->consumed);
            }
            else {
                printf("Product from 2 Processed: %d\n", buff->consumed);
            }
        }
        
        //Ends loop and sends signal if on last iteration
        if (buff->consumed == 150){
            pthread_cond_signal(&buff->empty);
            pthread_mutex_unlock(&buff->mutex);
            printf("Worker Thread %ld Done\n", pthread_self());
            return 0;
        }

        //Signal to Distributor that a product was just consumed
        pthread_cond_signal(&buff->empty);
        pthread_mutex_unlock(&buff->mutex);
        
    }

    return 0;
}