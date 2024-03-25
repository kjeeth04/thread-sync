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

/*  Function for putting product into the circular queue, and then incrementing head value. No return type
        @param buffer : buffer to be modified
        @param val : value to be inserted into buffer
        @param size : size of buffer
        @param head : location within buffer to insert value
        @param filled : amount of positions filled
*/
void put(int* buffer, int val, int* size, int* head, int* filled){
    buffer[*head] = val;
    //Moves head value forward in circular queue, increment filled count
    *head = (*head + 1) % *size;
    ++(*filled);
}